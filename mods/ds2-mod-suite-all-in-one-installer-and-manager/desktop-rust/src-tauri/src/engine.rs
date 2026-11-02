use crate::{
    ini::Ini,
    model::{Data, Profile, Setting, Values},
    platform::*,
};
use fs2::FileExt;
use serde::{Deserialize, Serialize};
use std::{
    collections::{BTreeMap, BTreeSet},
    fs::{self, OpenOptions},
    path::{Path, PathBuf},
};

#[derive(Clone, Default)]
pub struct Snapshot {
    pub files: BTreeMap<String, Vec<u8>>,
    pub hashes: BTreeMap<String, Option<String>>,
}
#[derive(Serialize)]
#[serde(rename_all = "camelCase")]
pub struct View {
    pub game_path: Option<String>,
    pub candidates: Vec<String>,
    pub supported: bool,
    pub running: bool,
    pub values: Values,
    pub installed: Vec<String>,
    pub warnings: Vec<String>,
    pub payload_ready: bool,
    pub backup_available: bool,
}
#[derive(Clone)]
struct Change {
    name: String,
    before: Option<Vec<u8>>,
    after: Option<Vec<u8>>,
    before_hash: Option<String>,
    after_hash: Option<String>,
    reason: String,
}
#[derive(Clone)]
struct Plan {
    token: String,
    changes: Vec<Change>,
    expected: BTreeMap<String, Option<String>>,
    values: Values,
}
#[derive(Serialize)]
pub struct PlanView {
    pub token: String,
    pub changes: Vec<ChangeView>,
    pub selected: Vec<String>,
}
#[derive(Serialize)]
pub struct ChangeView {
    pub name: String,
    pub reason: String,
}
#[derive(Clone, Serialize, Deserialize)]
struct Journal {
    version: u32,
    id: String,
    status: String,
    changes: Vec<JournalChange>,
}
#[derive(Clone, Serialize, Deserialize)]
struct JournalChange {
    name: String,
    before_hash: Option<String>,
    after_hash: Option<String>,
}
pub struct Engine {
    pub data: Data,
    pub package: PathBuf,
    pub game: Option<PathBuf>,
    pub snapshot: Option<Snapshot>,
    plan: Option<Plan>,
    pub supported: bool,
    pub settings_dir: PathBuf,
}
impl Engine {
    pub fn new() -> Self {
        Self {
            data: Data::load(),
            package: std::env::current_exe()
                .unwrap()
                .parent()
                .unwrap()
                .to_path_buf(),
            game: None,
            snapshot: None,
            plan: None,
            supported: false,
            settings_dir: user_dir(),
        }
    }
    pub fn verify_package(&self) -> Result<serde_json::Value, String> {
        let mut count = 0;
        for m in &self.data.catalog.mods {
            for f in &m.files {
                self.payload(&f.payload, &f.sha256)?;
                count += 1;
            }
        }
        self.payload(
            &self.data.catalog.loader.payload,
            &self.data.catalog.loader.sha256,
        )?;
        self.data.validate_values(&self.data.defaults())?;
        Ok(
            serde_json::json!({"ok":true,"suiteVersion":self.data.catalog.suite_version,"components":self.data.catalog.mods.len(),"settings":self.data.fields.len(),"verifiedPayloadFiles":count+1}),
        )
    }
    pub fn inspect(&mut self, path: Option<PathBuf>) -> Result<View, String> {
        self.plan = None;
        let candidates = discover(&self.data.catalog.game.steam_folder);
        if let Some(p) = path {
            if !p.is_absolute() {
                return Err("Choose an absolute game path".into());
            }
            check_path(&p)?;
            self.game = Some(p);
        } else if self.game.is_none() {
            self.game = candidates.first().cloned();
        }
        let mut values = self.data.defaults();
        let mut warnings = Vec::new();
        for folder in [legacy_dir(), self.settings_dir.clone()] {
            if let Some(raw) = read(&folder.join("mod-settings.json"))? {
                if let Ok(p) = serde_json::from_slice::<Profile>(&raw) {
                    if p.schema_version == 1 {
                        for item in p.values {
                            if let Some(f) = self
                                .data
                                .fields
                                .iter()
                                .find(|f| f.id.eq_ignore_ascii_case(&item.id))
                            {
                                if let Ok(v) = f.normalize(&item.value) {
                                    values.insert(f.id.clone(), v);
                                }
                            }
                        }
                    }
                }
            }
        }
        let mut installed = Vec::new();
        self.supported = false;
        let mut backup_available = false;
        if let Some(game) = &self.game {
            check_path(game)?;
            let exe = game.join(&self.data.catalog.game.executable);
            if !exe.is_file() {
                return Err("DS2.exe was not found in the selected folder".into());
            }
            let game_hash = hash_file(&exe)?;
            self.supported = self
                .data
                .catalog
                .game
                .supported_versions
                .iter()
                .any(|b| b.sha256.eq_ignore_ascii_case(&game_hash));
            if !self.supported {
                warnings.push("Unsupported game build: installation is disabled.".into());
            }
            let snapshot = self.scan(game)?;
            for m in &self.data.catalog.mods {
                let binaries: Vec<_> = m.files.iter().filter(|f| f.kind != "config").collect();
                if !binaries.is_empty()
                    && binaries
                        .iter()
                        .all(|f| snapshot.files.contains_key(&f.target))
                {
                    installed.push(m.id.clone());
                }
                for f in binaries {
                    if let Some(Some(h)) = snapshot.hashes.get(&f.target) {
                        if !std::iter::once(&f.sha256)
                            .chain(f.previous_hashes.iter())
                            .any(|known| known.eq_ignore_ascii_case(h))
                        {
                            warnings.push(format!(
                                "Unknown or modified file will be retained: {}",
                                f.target
                            ));
                        }
                    }
                }
            }
            for f in &self.data.fields {
                if let Some(raw) = snapshot.files.get(&f.target) {
                    let source = decode(raw)?;
                    if let Some(v) = Ini::parse(&source).get(f) {
                        match f.normalize(&v) {
                            Ok(v) => {
                                values.insert(f.id.clone(), v);
                            }
                            Err(_) => warnings.push(format!(
                                "Invalid setting needs review: {} / {}",
                                f.target, f.key
                            )),
                        }
                    }
                }
            }
            backup_available = self.latest_journal(game)?.is_some();
            self.snapshot = Some(snapshot);
        } else {
            self.snapshot = None;
        }
        let payload_ready = self
            .data
            .catalog
            .mods
            .iter()
            .flat_map(|m| &m.files)
            .filter(|f| f.kind != "config")
            .all(|f| self.package.join(&f.payload).is_file())
            && self
                .package
                .join(&self.data.catalog.loader.payload)
                .is_file();
        Ok(View {
            game_path: self.game.as_ref().map(|p| p.to_string_lossy().into_owned()),
            candidates: candidates
                .iter()
                .map(|p| p.to_string_lossy().into_owned())
                .collect(),
            supported: self.supported,
            running: game_running()?,
            values,
            installed,
            warnings,
            payload_ready,
            backup_available,
        })
    }
    fn scan(&self, game: &Path) -> Result<Snapshot, String> {
        let mut out = Snapshot::default();
        for name in self.data.inspected() {
            safe_name(&name)?;
            let value = read(&game.join(&name))?;
            out.hashes
                .insert(name.clone(), value.as_ref().map(|v| hash(v)));
            if let Some(v) = value {
                out.files.insert(name, v);
            }
        }
        Ok(out)
    }
    fn payload(&self, path: &str, expected: &str) -> Result<Vec<u8>, String> {
        let relative = Path::new(path);
        if relative.is_absolute()
            || relative
                .components()
                .any(|c| !matches!(c, std::path::Component::Normal(_)))
        {
            return Err("Invalid payload path".into());
        }
        let raw = read(&self.package.join(relative))?.ok_or_else(|| {
            format!(
                "Missing bundled file: {}. Extract the complete release ZIP first.",
                path
            )
        })?;
        if !hash(&raw).eq_ignore_ascii_case(expected) {
            return Err(format!("Bundled file hash mismatch: {}", path));
        }
        Ok(raw)
    }
    pub fn preview(&mut self, ids: Vec<String>, values: Values) -> Result<PlanView, String> {
        self.plan = None;
        if let Some(game) = &self.game {
            if self
                .latest_journal(game)?
                .is_some_and(|(_, j)| j.status != "complete")
            {
                return Err("A previous operation was interrupted. Use Restore backup before applying further changes.".into());
            }
        }
        let values = self.data.validate_values(&values)?;
        let selected = self.data.selected(&ids)?;
        let snapshot = self.snapshot.as_ref().ok_or("Select a game folder first")?;
        if !self.supported {
            return Err("The selected game build is not supported".into());
        }
        if game_running()? {
            return Err("Close Death Stranding 2 before changing mods".into());
        }
        let mut planned: BTreeMap<String, (Option<Vec<u8>>, String)> = BTreeMap::new();
        let wanted: BTreeSet<_> = self
            .data
            .catalog
            .mods
            .iter()
            .filter(|m| selected.contains(&m.id))
            .flat_map(|m| m.files.iter().map(|f| f.target.clone()))
            .collect();
        for m in &self.data.catalog.mods {
            let enabled = selected.contains(&m.id);
            if enabled {
                for conflict in &m.conflicting_files {
                    if snapshot.files.contains_key(conflict) {
                        return Err(format!("{} — conflicting file: {}", m.name, conflict));
                    }
                }
            }
            for f in &m.files {
                if f.kind == "config" {
                    if enabled {
                        let source = match snapshot.files.get(&f.target) {
                            Some(raw) => decode(raw)?,
                            None => self
                                .data
                                .templates
                                .get(&f.target)
                                .ok_or("Missing INI template")?
                                .clone(),
                        };
                        let mut ini = Ini::parse(&source);
                        for field in self
                            .data
                            .fields
                            .iter()
                            .filter(|field| field.target == f.target)
                        {
                            ini.set(field, values.get(&field.id).ok_or("Incomplete settings")?);
                        }
                        planned.insert(
                            f.target.clone(),
                            (Some(ini.render().into_bytes()), "Save configuration".into()),
                        );
                    }
                    continue;
                }
                let installed = snapshot.hashes.get(&f.target).and_then(|h| h.as_ref());
                let known = installed.is_none_or(|h| {
                    std::iter::once(&f.sha256)
                        .chain(f.previous_hashes.iter())
                        .any(|x| x.eq_ignore_ascii_case(h))
                });
                if !known {
                    if enabled {
                        return Err(format!("Unknown or modified file retained: {}", f.target));
                    }
                    continue;
                }
                if enabled {
                    if installed.is_none_or(|h| !h.eq_ignore_ascii_case(&f.sha256)) {
                        planned.insert(
                            f.target.clone(),
                            (
                                Some(self.payload(&f.payload, &f.sha256)?),
                                "Install / update".into(),
                            ),
                        );
                    }
                } else if installed.is_some() && !wanted.contains(&f.target) {
                    planned.insert(f.target.clone(), (None, "Remove known mod binary".into()));
                }
            }
            for old in &m.obsolete_files {
                if let Some(Some(h)) = snapshot.hashes.get(&old.target) {
                    if !h.eq_ignore_ascii_case(&old.sha256) {
                        if enabled {
                            return Err(format!("Unknown obsolete file retained: {}", old.target));
                        }
                    } else if !wanted.contains(&old.target) {
                        planned.insert(
                            old.target.clone(),
                            (None, "Remove superseded binary".into()),
                        );
                    }
                }
            }
        }
        if !selected.is_empty() {
            let l = &self.data.catalog.loader;
            let proxies: Vec<_> = l
                .proxy_file_names
                .iter()
                .filter(|n| snapshot.files.contains_key(*n))
                .collect();
            if proxies.len() > 1 {
                return Err(
                    "Multiple ASI proxy loaders found. Resolve this conflict first.".into(),
                );
            }
            if let Some(proxy) = proxies.first() {
                let h = snapshot
                    .hashes
                    .get(*proxy)
                    .and_then(|h| h.as_ref())
                    .unwrap();
                if !std::iter::once(&l.sha256)
                    .chain(l.known_compatible_hashes.iter())
                    .any(|known| known.eq_ignore_ascii_case(h))
                {
                    return Err(format!("Unknown loader retained: {}", proxy));
                }
            } else {
                planned.insert(
                    l.file_name.clone(),
                    (
                        Some(self.payload(&l.payload, &l.sha256)?),
                        "Install ASI loader".into(),
                    ),
                );
            }
        }
        let mut changes = Vec::new();
        for (name, (after, reason)) in planned {
            let before = snapshot.files.get(&name).cloned();
            let before_hash = before.as_ref().map(|b| hash(b));
            let after_hash = after.as_ref().map(|b| hash(b));
            if before_hash != after_hash {
                changes.push(Change {
                    name,
                    before,
                    after,
                    before_hash,
                    after_hash,
                    reason,
                });
            }
        }
        let token = id();
        let result = PlanView {
            token: token.clone(),
            changes: changes
                .iter()
                .map(|c| ChangeView {
                    name: c.name.clone(),
                    reason: c.reason.clone(),
                })
                .collect(),
            selected: selected.into_iter().collect(),
        };
        self.plan = Some(Plan {
            token,
            changes,
            expected: snapshot.hashes.clone(),
            values,
        });
        Ok(result)
    }
    fn assert_snapshot(
        &self,
        game: &Path,
        expected: &BTreeMap<String, Option<String>>,
    ) -> Result<(), String> {
        for (name, h) in expected {
            if read(&game.join(name))?.as_ref().map(|v| hash(v)) != *h {
                return Err(format!(
                    "File changed after inspection: {}. Read the game folder again.",
                    name
                ));
            }
        }
        Ok(())
    }
    pub fn apply(&mut self, token: &str) -> Result<String, String> {
        let plan = self.plan.take().ok_or("Preview changes first")?;
        if plan.token != token {
            return Err("Stale installation plan".into());
        }
        let game = self.game.clone().ok_or("Select game folder")?;
        let game_hash = hash_file(&game.join(&self.data.catalog.game.executable))?;
        if !self
            .data
            .catalog
            .game
            .supported_versions
            .iter()
            .any(|b| b.sha256.eq_ignore_ascii_case(&game_hash))
        {
            return Err("Game build changed".into());
        }
        if game_running()? {
            return Err("Close Death Stranding 2 first".into());
        }
        let result = self.commit(&game, &plan.changes, &plan.expected, None)?;
        self.snapshot = None;
        let profile = Profile {
            schema_version: 1,
            values: plan
                .values
                .into_iter()
                .map(|(id, value)| Setting { id, value })
                .collect(),
        };
        check_path(&self.settings_dir)?;
        fs::create_dir_all(&self.settings_dir).map_err(err)?;
        let save = (|| {
            atomic_write(
                &self.settings_dir.join("mod-settings.json"),
                &serde_json::to_vec_pretty(&profile).map_err(err)?,
            )?;
            atomic_write(
                &self.settings_dir.join("settings.json"),
                &serde_json::to_vec(&serde_json::json!({"gamePath":game.to_string_lossy()}))
                    .map_err(err)?,
            )
        })();
        Ok(if let Err(e) = save {
            format!("{}; settings profile could not be saved: {}", result, e)
        } else {
            result
        })
    }
    fn commit(
        &self,
        game: &Path,
        changes: &[Change],
        expected: &BTreeMap<String, Option<String>>,
        fail_after: Option<usize>,
    ) -> Result<String, String> {
        check_path(game)?;
        let allowed = self.data.allowed();
        if changes.iter().any(|c| !allowed.contains(&c.name)) {
            return Err("Unmanaged write target rejected".into());
        }
        let root = game.join("DS2SuiteBackups");
        check_path(&root)?;
        fs::create_dir_all(&root).map_err(err)?;
        let lock_path = root.join("manager.lock");
        check_path(&lock_path)?;
        let lock = OpenOptions::new()
            .create(true)
            .read(true)
            .write(true)
            .open(&lock_path)
            .map_err(err)?;
        lock.try_lock_exclusive()
            .map_err(|_| "Another manager is applying changes".to_string())?;
        self.assert_snapshot(game, expected)?;
        if changes.is_empty() {
            return Ok("No changes needed".into());
        }
        let txn_id = id();
        let backup = root.join(&txn_id);
        fs::create_dir(&backup).map_err(err)?;
        let mut journal = Journal {
            version: 1,
            id: txn_id.clone(),
            status: "prepared".into(),
            changes: changes
                .iter()
                .map(|c| JournalChange {
                    name: c.name.clone(),
                    before_hash: c.before_hash.clone(),
                    after_hash: c.after_hash.clone(),
                })
                .collect(),
        };
        for c in changes {
            if let Some(before) = &c.before {
                atomic_write(&backup.join(&c.name), before)?;
                if read(&backup.join(&c.name))?.as_ref().map(|v| hash(v)) != c.before_hash {
                    return Err("Backup verification failed".into());
                }
            }
        }
        let save = |j: &Journal| {
            atomic_write(
                &backup.join("transaction.json"),
                &serde_json::to_vec_pretty(j).map_err(err)?,
            )
        };
        save(&journal)?;
        self.assert_snapshot(game, expected)?;
        let mut touched = Vec::new();
        let result = (|| {
            journal.status = "applying".into();
            save(&journal)?;
            for (index, c) in changes.iter().enumerate() {
                if fail_after == Some(index) {
                    return Err("Injected test failure".into());
                }
                if read(&game.join(&c.name))?.as_ref().map(|v| hash(v)) != c.before_hash {
                    return Err(format!("Concurrent file change: {}", c.name));
                }
                touched.push(c);
                set_file(&game.join(&c.name), c.after.as_deref())?;
                if read(&game.join(&c.name))?.as_ref().map(|v| hash(v)) != c.after_hash {
                    return Err(format!("Write verification failed: {}", c.name));
                }
            }
            journal.status = "complete".into();
            save(&journal)
        })();
        if let Err(e) = result {
            let mut failed = Vec::new();
            for c in touched.into_iter().rev() {
                let rollback = (|| {
                    let current = read(&game.join(&c.name))?.as_ref().map(|v| hash(v));
                    if current == c.before_hash {
                        return Ok(());
                    }
                    if current != c.after_hash {
                        return Err("Concurrent change during rollback".into());
                    }
                    set_file(&game.join(&c.name), c.before.as_deref())?;
                    if read(&game.join(&c.name))?.as_ref().map(|v| hash(v)) != c.before_hash {
                        return Err("Rollback verification failed".into());
                    }
                    Ok::<(), String>(())
                })();
                if rollback.is_err() {
                    failed.push(c.name.clone());
                }
            }
            journal.status = if failed.is_empty() {
                "rolled-back"
            } else {
                "recovery-required"
            }
            .into();
            let _ = save(&journal);
            return Err(format!(
                "{}; {}. Backup: {} {}",
                e,
                journal.status,
                backup.display(),
                failed.join(", ")
            ));
        }
        Ok(format!(
            "{} files updated. Backup: {}",
            changes.len(),
            backup.display()
        ))
    }
    fn latest_journal(&self, game: &Path) -> Result<Option<(PathBuf, Journal)>, String> {
        let root = game.join("DS2SuiteBackups");
        check_path(&root)?;
        if !root.exists() {
            return Ok(None);
        }
        let mut journals = Vec::new();
        for entry in fs::read_dir(&root).map_err(err)? {
            let entry = entry.map_err(err)?;
            let name = entry.file_name().to_string_lossy().into_owned();
            if name.chars().all(|c| c.is_ascii_digit() || c == '-') && name.contains('-') {
                let path = entry.path();
                if let Some(raw) = read(&path.join("transaction.json"))? {
                    let j: Journal = serde_json::from_slice(&raw).map_err(err)?;
                    if j.version != 1 || j.id != name || j.changes.len() > 200 {
                        return Err("Invalid backup journal".into());
                    }
                    if ["complete", "applying", "recovery-required"].contains(&j.status.as_str()) {
                        journals.push((path, j));
                    }
                }
            }
        }
        journals.sort_by(|a, b| b.1.id.cmp(&a.1.id));
        Ok(journals.into_iter().next())
    }
    pub fn restore_latest(&mut self) -> Result<String, String> {
        self.plan = None;
        if game_running()? {
            return Err("Close Death Stranding 2 first".into());
        }
        let game = self.game.clone().ok_or("Select game folder")?;
        let (backup, j) = self.latest_journal(&game)?.ok_or("No backup available")?;
        let allowed = self.data.allowed();
        let mut changes = Vec::new();
        let mut expected = BTreeMap::new();
        for c in j.changes {
            if !allowed.contains(&c.name) || expected.contains_key(&c.name) {
                return Err("Invalid backup target".into());
            }
            let current = read(&game.join(&c.name))?;
            let h = current.as_ref().map(|b| hash(b));
            expected.insert(c.name.clone(), h.clone());
            if h == c.before_hash {
                continue;
            }
            if h != c.after_hash {
                return Err(format!("Changed file retained: {}", c.name));
            }
            let original = if c.before_hash.is_some() {
                read(&backup.join(&c.name))?
            } else {
                None
            };
            if original.as_ref().map(|b| hash(b)) != c.before_hash {
                return Err("Damaged backup".into());
            }
            changes.push(Change {
                name: c.name,
                before: current,
                after: original,
                before_hash: h,
                after_hash: c.before_hash,
                reason: "Restore".into(),
            });
        }
        let result = self.commit(&game, &changes, &expected, None)?;
        self.snapshot = None;
        Ok(result)
    }
}
fn decode(raw: &[u8]) -> Result<String, String> {
    if raw.starts_with(&[0xFF, 0xFE]) {
        if raw.len() % 2 != 0 {
            return Err("Invalid UTF-16 INI".into());
        }
        return String::from_utf16(
            &raw[2..]
                .chunks_exact(2)
                .map(|b| u16::from_le_bytes([b[0], b[1]]))
                .collect::<Vec<_>>(),
        )
        .map_err(err);
    }
    String::from_utf8(raw.to_vec())
        .map_err(|_| "Unsupported INI encoding; file retained unchanged".into())
}

#[cfg(test)]
mod tests;
