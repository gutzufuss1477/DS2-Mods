use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::collections::{BTreeMap, BTreeSet};

#[derive(Clone, Deserialize)]
pub struct Data {
    pub catalog: Catalog,
    pub fields: Vec<Field>,
    pub templates: BTreeMap<String, String>,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Catalog {
    pub suite_version: String,
    pub game: Game,
    pub loader: Loader,
    pub mods: Vec<Mod>,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Game {
    pub executable: String,
    pub steam_folder: String,
    pub supported_versions: Vec<Build>,
}
#[derive(Clone, Deserialize)]
pub struct Build {
    pub sha256: String,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Loader {
    pub file_name: String,
    pub payload: String,
    pub sha256: String,
    #[serde(default)]
    pub known_compatible_hashes: Vec<String>,
    pub proxy_file_names: Vec<String>,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Mod {
    pub id: String,
    pub name: String,
    pub files: Vec<ModFile>,
    #[serde(default)]
    pub requires: Vec<String>,
    pub exclusive_group: Option<String>,
    #[serde(default)]
    pub setting_overrides: BTreeMap<String, String>,
    #[serde(default)]
    pub conflicting_files: Vec<String>,
    #[serde(default)]
    pub obsolete_files: Vec<Obsolete>,
}
#[derive(Clone, Deserialize)]
pub struct Obsolete {
    pub target: String,
    pub sha256: String,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct ModFile {
    pub payload: String,
    pub target: String,
    pub sha256: String,
    pub kind: String,
    #[serde(default)]
    pub previous_hashes: Vec<String>,
}
#[derive(Clone, Deserialize)]
#[serde(rename_all = "camelCase")]
pub struct Field {
    pub id: String,
    pub mod_id: String,
    pub target: String,
    pub section: String,
    pub key: String,
    #[serde(rename = "type")]
    pub kind: String,
    pub default_value: String,
    #[serde(default)]
    pub aliases: Vec<String>,
    #[serde(default)]
    pub choices: Vec<String>,
    pub min: Option<Value>,
    pub max: Option<Value>,
    pub multiple_of: Option<Value>,
}
#[derive(Clone, Deserialize, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Profile {
    pub schema_version: u32,
    pub values: Vec<Setting>,
}
#[derive(Clone, Deserialize, Serialize)]
pub struct Setting {
    pub id: String,
    pub value: String,
}
pub type Values = BTreeMap<String, String>;

impl Field {
    pub fn normalize(&self, input: &str) -> Result<String, String> {
        let v = input.trim();
        let error = || format!("Invalid value for {} / {}", self.section, self.key);
        if v.is_empty() || v.len() > 128 || v.chars().any(|c| "\r\n[]=;".contains(c)) {
            return Err(error());
        }
        if self.kind == "bool" {
            return match v.to_ascii_lowercase().as_str() {
                "true" | "1" => Ok("1".into()),
                "false" | "0" => Ok("0".into()),
                _ => Err(error()),
            };
        }
        if self.kind == "choice" {
            return self
                .choices
                .iter()
                .find(|c| c.eq_ignore_ascii_case(v))
                .cloned()
                .ok_or_else(error);
        }
        if self.kind == "nativeOrInt" && v.eq_ignore_ascii_case("native") {
            return Ok("Native".into());
        }
        if ["int", "uint64", "nativeOrInt"].contains(&self.kind.as_str()) {
            let n: i128 = v.parse().map_err(|_| error())?;
            let (min, max) = if self.kind == "uint64" {
                (0, u64::MAX as i128)
            } else {
                (i64::MIN as i128, i64::MAX as i128)
            };
            let bound =
                |x: &Option<Value>| x.as_ref().and_then(|x| x.to_string().parse::<i128>().ok());
            if n < min
                || n > max
                || bound(&self.min).is_some_and(|b| n < b)
                || bound(&self.max).is_some_and(|b| n > b)
                || bound(&self.multiple_of).is_some_and(|b| b == 0 || n % b != 0)
            {
                return Err(error());
            }
            return Ok(n.to_string());
        }
        if self.kind != "decimal" {
            return Err(error());
        }
        let v = v.replace(',', ".");
        if !v.chars().all(|c| c.is_ascii_digit() || "+-.".contains(c)) {
            return Err(error());
        }
        let n: f64 = v.parse().map_err(|_| error())?;
        let bound = |x: &Option<Value>| x.as_ref().and_then(Value::as_f64);
        if !n.is_finite()
            || bound(&self.min).is_some_and(|b| n < b)
            || bound(&self.max).is_some_and(|b| n > b)
            || bound(&self.multiple_of)
                .is_some_and(|b| b <= 0.0 || ((n / b).round() - n / b).abs() > 1e-8)
        {
            return Err(error());
        }
        Ok(n.to_string())
    }
}
impl Data {
    // Shared targets (variant ASIs) are owned by the catalog, not by whichever
    // variant happens to be selected. Unknown files still fail closed.
    pub fn known_binary(&self, target: &str, hash: &str) -> bool {
        self.catalog.mods.iter().flat_map(|m| &m.files).any(|f| {
            f.kind != "config"
                && f.target == target
                && std::iter::once(&f.sha256)
                    .chain(f.previous_hashes.iter())
                    .any(|h| h.eq_ignore_ascii_case(hash))
        })
    }

    pub fn import_profile(&self, profile: Profile) -> Result<Values, String> {
        if profile.schema_version != 1 {
            return Err("Unknown profile version".into());
        }
        let mut values = self.defaults();
        let mut seen = BTreeSet::new();
        for item in profile.values {
            if !seen.insert(item.id.to_ascii_lowercase()) {
                return Err("Duplicate setting in profile".into());
            }
            // Older managers exported retired diagnostic settings as well.
            // Import only recognized field IDs; never treat profile text as an INI key.
            if let Some(f) = self
                .fields
                .iter()
                .find(|f| f.id.eq_ignore_ascii_case(&item.id))
            {
                values.insert(f.id.clone(), f.normalize(&item.value)?);
            }
        }
        self.validate_values(&values)
    }

    pub fn load() -> Self {
        serde_json::from_str(include_str!("../generated/data.json"))
            .expect("Invalid embedded catalog")
    }
    pub fn defaults(&self) -> Values {
        self.fields
            .iter()
            .map(|f| (f.id.clone(), f.default_value.clone()))
            .collect()
    }
    pub fn validate_values(&self, input: &Values) -> Result<Values, String> {
        if input.len() != self.fields.len() {
            return Err("Incomplete settings profile".into());
        }
        let mut values = Values::new();
        for f in &self.fields {
            values.insert(
                f.id.clone(),
                f.normalize(input.get(&f.id).ok_or("Unknown settings profile")?)?,
            );
        }
        let pairs = [
            (
                "hill-assist-and-speed-boost",
                "HillAssist",
                "FullAssistSlopeDegrees",
                "AssistStartSlopeDegrees",
                true,
            ),
            (
                "hill-assist-and-speed-boost",
                "HillAssist",
                "ScanEndGroup",
                "ScanStartGroup",
                true,
            ),
            (
                "hill-assist-and-speed-boost",
                "VehicleSpeedBoost",
                "ScanEndGroup",
                "ScanStartGroup",
                true,
            ),
            (
                "zipline-range",
                "ZiplineRange",
                "Level2RangeMeters",
                "Level1RangeMeters",
                false,
            ),
            (
                "climbing-power-gloves-range",
                "ClimbingGlovesRange",
                "Level2RangeMeters",
                "Level1RangeMeters",
                false,
            ),
        ];
        for (mod_id, section, hi, lo, strict) in pairs {
            let number = |key: &str| -> Option<f64> {
                self.fields
                    .iter()
                    .find(|f| f.mod_id == mod_id && f.section == section && f.key == key)
                    .and_then(|f| values.get(&f.id))
                    .and_then(|s| s.parse().ok())
            };
            if let (Some(a), Some(b)) = (number(hi), number(lo)) {
                if a < b || (strict && a == b) {
                    return Err(format!(
                        "{}: {} must be {} {}",
                        section,
                        hi,
                        if strict { "greater than" } else { "at least" },
                        lo
                    ));
                }
            }
        }
        Ok(values)
    }
    pub fn selected(&self, ids: &[String]) -> Result<BTreeSet<String>, String> {
        fn add(
            data: &Data,
            id: &str,
            out: &mut BTreeSet<String>,
            stack: &mut BTreeSet<String>,
        ) -> Result<(), String> {
            if out.contains(id) {
                return Ok(());
            }
            if !stack.insert(id.into()) {
                return Err("Dependency cycle".into());
            }
            let m = data
                .catalog
                .mods
                .iter()
                .find(|m| m.id == id)
                .ok_or("Unknown component")?;
            for dep in &m.requires {
                add(data, dep, out, stack)?;
            }
            stack.remove(id);
            out.insert(id.into());
            Ok(())
        }
        let mut result = BTreeSet::new();
        for id in ids {
            add(self, id, &mut result, &mut BTreeSet::new())?;
        }
        let mut groups = BTreeSet::new();
        for m in &self.catalog.mods {
            if result.contains(&m.id) {
                if let Some(g) = &m.exclusive_group {
                    if !groups.insert(g) {
                        return Err(format!("Select only one variant: {}", g));
                    }
                }
            }
        }
        Ok(result)
    }
    pub fn allowed(&self) -> BTreeSet<String> {
        let mut out = BTreeSet::from([self.catalog.loader.file_name.clone()]);
        for m in &self.catalog.mods {
            out.extend(m.files.iter().map(|f| f.target.clone()));
            out.extend(m.obsolete_files.iter().map(|f| f.target.clone()));
        }
        out
    }
    pub fn inspected(&self) -> BTreeSet<String> {
        let mut out = self.allowed();
        out.extend(self.catalog.loader.proxy_file_names.clone());
        for m in &self.catalog.mods {
            out.extend(m.conflicting_files.clone());
        }
        out
    }
}
