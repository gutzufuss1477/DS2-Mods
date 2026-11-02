use super::*;
use tempfile::TempDir;

fn fixture() -> (TempDir, Engine) {
    let temp = tempfile::tempdir().unwrap();
    let mut e = Engine::new();
    e.package = temp.path().join("package");
    e.settings_dir = temp.path().join("profile");
    let game = temp.path().join("game");
    fs::create_dir_all(&game).unwrap();
    fs::write(game.join("DS2.exe"), b"test-game").unwrap();
    e.data.catalog.game.supported_versions[0].sha256 = hash(b"test-game");
    e.game = Some(game.clone());
    e.supported = true;
    for m in &mut e.data.catalog.mods {
        for f in &mut m.files {
            if f.kind != "config" {
                let raw = format!("test-payload:{}:{}", m.id, f.target).into_bytes();
                f.sha256 = hash(&raw);
                let p = e.package.join(&f.payload);
                fs::create_dir_all(p.parent().unwrap()).unwrap();
                fs::write(p, raw).unwrap();
            }
        }
    }
    let l = &mut e.data.catalog.loader;
    l.sha256 = hash(b"test-loader");
    l.known_compatible_hashes = vec![l.sha256.clone()];
    let p = e.package.join(&l.payload);
    fs::create_dir_all(p.parent().unwrap()).unwrap();
    fs::write(p, b"test-loader").unwrap();
    e.snapshot = Some(e.scan(&game).unwrap());
    (temp, e)
}
fn chosen(e: &Engine) -> Vec<String> {
    let mut groups = BTreeSet::new();
    e.data
        .catalog
        .mods
        .iter()
        .filter(|m| {
            m.exclusive_group
                .as_ref()
                .is_none_or(|g| groups.insert(g.clone()))
        })
        .map(|m| m.id.clone())
        .collect()
}

#[test]
fn all_defaults_are_valid_and_complete() {
    let data = Data::load();
    assert_eq!(data.fields.len(), 398);
    assert_eq!(data.templates.len(), 24);
    assert_eq!(data.catalog.mods.len(), 29);
    data.validate_values(&data.defaults()).unwrap();
}
#[test]
fn invalid_values_and_relationships_fail() {
    let data = Data::load();
    let mut v = data.defaults();
    let f = data.fields.iter().find(|f| f.kind == "decimal").unwrap();
    v.insert(f.id.clone(), "NaN".into());
    assert!(data.validate_values(&v).is_err());
    v = data.defaults();
    v.pop_first();
    assert!(data.validate_values(&v).is_err());
    v = data.defaults();
    let f = data
        .fields
        .iter()
        .find(|f| f.section == "HillAssist" && f.key == "FullAssistSlopeDegrees")
        .unwrap();
    v.insert(f.id.clone(), "1".into());
    assert!(data.validate_values(&v).is_err());
}
#[test]
fn uint64_preserves_exact_large_values() {
    let mut f = Data::load().fields[0].clone();
    f.kind = "uint64".into();
    f.min = None;
    f.max = None;
    f.multiple_of = None;
    assert_eq!(
        f.normalize("18446744073709551615").unwrap(),
        "18446744073709551615"
    );
    assert!(f.normalize("18446744073709551616").is_err());
    assert!(f.normalize("-1").is_err());
}
#[test]
fn dependencies_and_exclusivity() {
    let data = Data::load();
    let add = data
        .catalog
        .mods
        .iter()
        .find(|m| !m.requires.is_empty())
        .unwrap();
    let selected = data.selected(&[add.id.clone()]).unwrap();
    for dep in &add.requires {
        assert!(selected.contains(dep));
    }
    let m = data
        .catalog
        .mods
        .iter()
        .find(|m| m.exclusive_group.is_some())
        .unwrap();
    let group = m.exclusive_group.clone();
    let ids: Vec<_> = data
        .catalog
        .mods
        .iter()
        .filter(|m| m.exclusive_group == group)
        .map(|m| m.id.clone())
        .collect();
    assert!(data.selected(&ids).is_err());
    assert!(data.selected(&["unknown".into()]).is_err());
}
#[test]
fn ini_preserves_comments_unknown_keys_and_migrates_aliases() {
    let data = Data::load();
    let f = data.fields.iter().find(|f| !f.aliases.is_empty()).unwrap();
    let source=format!("; personal configuration\r\n[{}]\r\n{}=123 ; preserve this comment\r\n{}=222\r\nPersonalKey=56789\r\n\r\n[StoryData]\r\nCustomCoffin=987654321012345\r\n",f.section,f.aliases[0],f.key);
    let mut ini = Ini::parse(&source);
    ini.set(f, "150");
    let out = ini.render();
    assert!(out.contains("; personal configuration\r\n"));
    assert!(out.contains("; preserve this comment"));
    assert!(out.contains("PersonalKey=56789"));
    assert!(out.contains("CustomCoffin=987654321012345"));
    assert!(!out.contains(&format!("{}=", f.aliases[0])));
    assert_eq!(Ini::parse(&out).get(f).unwrap(), "150");
    assert_eq!(out.matches(&format!("{}=", f.key)).count(), 1);
}
#[test]
fn all_components_install_remove_and_restore() {
    let (_temp, mut e) = fixture();
    let ids = chosen(&e);
    let plan = e.preview(ids, e.data.defaults()).unwrap();
    assert!(plan.changes.len() > 45);
    e.apply(&plan.token).unwrap();
    let game = e.game.clone().unwrap();
    for m in &e.data.catalog.mods {
        if plan.selected.contains(&m.id) {
            for f in &m.files {
                assert!(game.join(&f.target).is_file(), "{}", f.target);
            }
        }
    }
    e.snapshot = Some(e.scan(&game).unwrap());
    let remove = e.preview(Vec::new(), e.data.defaults()).unwrap();
    e.apply(&remove.token).unwrap();
    assert!(game.join(&e.data.catalog.loader.file_name).exists());
    for m in &e.data.catalog.mods {
        for f in &m.files {
            if f.kind != "config" {
                assert!(!game.join(&f.target).exists());
            }
        }
    }
    e.restore_latest().unwrap();
    for m in &e.data.catalog.mods {
        if plan.selected.contains(&m.id) {
            for f in &m.files {
                assert!(game.join(&f.target).exists());
            }
        }
    }
}
#[test]
fn payload_tamper_blocks_before_game_writes() {
    let (_temp, mut e) = fixture();
    let m = e.data.catalog.mods[0].clone();
    let f = m.files.iter().find(|f| f.kind != "config").unwrap();
    fs::write(e.package.join(&f.payload), b"modified").unwrap();
    assert!(e.preview(vec![m.id], e.data.defaults()).is_err());
    assert_eq!(fs::read_dir(e.game.as_ref().unwrap()).unwrap().count(), 1);
}
#[test]
fn unknown_existing_binary_is_never_overwritten() {
    let (_temp, mut e) = fixture();
    let m = e.data.catalog.mods[0].clone();
    let f = m.files.iter().find(|f| f.kind != "config").unwrap();
    let game = e.game.clone().unwrap();
    fs::write(game.join(&f.target), b"third-party build").unwrap();
    e.snapshot = Some(e.scan(&game).unwrap());
    assert!(e.preview(vec![m.id], e.data.defaults()).is_err());
    let plan = e.preview(Vec::new(), e.data.defaults()).unwrap();
    assert!(plan.changes.is_empty());
    assert_eq!(
        fs::read(game.join(&f.target)).unwrap(),
        b"third-party build"
    );
}
#[test]
fn stale_plan_does_not_overwrite_external_edit() {
    let (_temp, mut e) = fixture();
    let plan = e
        .preview(vec![e.data.catalog.mods[0].id.clone()], e.data.defaults())
        .unwrap();
    let c = e.plan.as_ref().unwrap().changes[0].clone();
    let game = e.game.clone().unwrap();
    fs::write(game.join(&c.name), b"external edit").unwrap();
    assert!(e.apply(&plan.token).is_err());
    assert_eq!(fs::read(game.join(&c.name)).unwrap(), b"external edit");
}
#[test]
fn interrupted_installation_rolls_back() {
    let (_temp, mut e) = fixture();
    e.preview(chosen(&e), e.data.defaults()).unwrap();
    let plan = e.plan.clone().unwrap();
    let game = e.game.clone().unwrap();
    let result = e.commit(&game, &plan.changes, &plan.expected, Some(3));
    assert!(result.unwrap_err().contains("rolled-back"));
    for c in &plan.changes {
        assert_eq!(read(&game.join(&c.name)).unwrap(), c.before);
    }
    assert!(e.latest_journal(&game).unwrap().is_none());
}
#[test]
fn backup_restore_rejects_modified_files() {
    let (_temp, mut e) = fixture();
    let p = e
        .preview(vec![e.data.catalog.mods[0].id.clone()], e.data.defaults())
        .unwrap();
    let name = e.plan.as_ref().unwrap().changes[0].name.clone();
    e.apply(&p.token).unwrap();
    fs::write(e.game.as_ref().unwrap().join(&name), b"later user edit").unwrap();
    assert!(e.restore_latest().is_err());
    assert_eq!(
        fs::read(e.game.as_ref().unwrap().join(name)).unwrap(),
        b"later user edit"
    );
}
#[test]
fn old_binary_updates_preserve_personal_ini() {
    let (_temp, mut e) = fixture();
    let m = e.data.catalog.mods[0].clone();
    let binary = m.files.iter().find(|f| f.kind != "config").unwrap();
    let ini = m.files.iter().find(|f| f.kind == "config").unwrap();
    let old = b"old-supported-build";
    e.data.catalog.mods[0]
        .files
        .iter_mut()
        .find(|f| f.target == binary.target)
        .unwrap()
        .previous_hashes
        .push(hash(old));
    let game = e.game.clone().unwrap();
    fs::write(game.join(&binary.target), old).unwrap();
    let contents = format!(
        "{}\r\n[Personal]\r\nNote=keep-me\r\n",
        e.data.templates[&ini.target]
    );
    fs::write(game.join(&ini.target), contents).unwrap();
    e.snapshot = Some(e.scan(&game).unwrap());
    let p = e.preview(vec![m.id], e.data.defaults()).unwrap();
    e.apply(&p.token).unwrap();
    assert_eq!(
        hash_file(&game.join(&binary.target)).unwrap(),
        binary.sha256
    );
    assert!(fs::read_to_string(game.join(&ini.target))
        .unwrap()
        .contains("Note=keep-me"));
}
#[test]
fn dangerous_names_and_paths_are_rejected() {
    for name in [
        "../DS2.exe",
        "C:\\Windows\\file",
        "con.ini",
        "thing:stream",
        "name.",
        ".hidden",
    ] {
        assert!(safe_name(name).is_err());
    }
    assert!(check_path(Path::new("relative")).is_err());
    safe_name("DS2_APAS.ini").unwrap();
}
#[test]
fn apas_update_preserves_all_settings_and_restores_previous_build() {
    let (_temp, mut e) = fixture();
    let m = e
        .data
        .catalog
        .mods
        .iter()
        .find(|m| m.id == "apas-memory-costs")
        .unwrap()
        .clone();
    let binary = m.files.iter().find(|f| f.kind == "binary").unwrap();
    let old = b"previous-apas-test-build";
    e.data
        .catalog
        .mods
        .iter_mut()
        .find(|m| m.id == "apas-memory-costs")
        .unwrap()
        .files
        .iter_mut()
        .find(|f| f.kind == "binary")
        .unwrap()
        .previous_hashes
        .push(hash(old));
    let game = e.game.clone().unwrap();
    let ini_name = "ds2_apas_memory_costs.ini";
    let previous = "; Personal APAS settings\r\n[APASMemoryCosts]\r\nEnabled=0\r\nGlobalCost=25 ; chosen cost\r\n[APASUnlocks]\r\nUnlockAll=1\r\nEarlyAccess=1\r\n[Personal]\r\nNote=keep-me\r\n";
    fs::write(game.join(&binary.target), old).unwrap();
    fs::write(game.join(ini_name), previous).unwrap();
    let view = e.inspect(Some(game.clone())).unwrap();
    let fields: Vec<_> = e
        .data
        .fields
        .iter()
        .filter(|f| f.mod_id == m.id)
        .cloned()
        .collect();
    assert_eq!(fields.len(), 4);
    for f in &fields {
        let expected = match f.key.as_str() {
            "Enabled" => "0",
            "GlobalCost" => "25",
            _ => "1",
        };
        assert_eq!(view.values[&f.id], expected);
    }
    let plan = e.preview(vec![m.id], view.values.clone()).unwrap();
    e.apply(&plan.token).unwrap();
    assert_eq!(
        hash_file(&game.join(&binary.target)).unwrap(),
        binary.sha256
    );
    let contents = fs::read_to_string(game.join(ini_name)).unwrap();
    assert!(contents.contains("Note=keep-me") && contents.contains("; chosen cost"));
    let ini = Ini::parse(&contents);
    for f in &fields {
        assert_eq!(ini.get(f).unwrap(), view.values[&f.id]);
    }
    e.restore_latest().unwrap();
    assert_eq!(fs::read(game.join(&binary.target)).unwrap(), old);
    assert_eq!(fs::read_to_string(game.join(ini_name)).unwrap(), previous);
}
#[test]
fn game_change_blocks_application() {
    let (_temp, mut e) = fixture();
    let p = e
        .preview(vec![e.data.catalog.mods[0].id.clone()], e.data.defaults())
        .unwrap();
    fs::write(
        e.game.as_ref().unwrap().join("DS2.exe"),
        b"different game version",
    )
    .unwrap();
    assert!(e.apply(&p.token).is_err());
}
