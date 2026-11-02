#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]
mod engine;
mod ini;
mod model;
mod platform;
use engine::Engine;
use std::sync::Mutex;
use tauri::{Manager, State};

#[tauri::command]
async fn inspect(
    path: Option<String>,
    state: State<'_, Mutex<Engine>>,
) -> Result<engine::View, String> {
    let mut engine = state.lock().map_err(|_| "Manager busy")?;
    engine.inspect(path.map(std::path::PathBuf::from))
}
#[tauri::command]
async fn choose_game() -> Result<Option<String>, String> {
    Ok(rfd::AsyncFileDialog::new()
        .set_title("Select the folder containing DS2.exe")
        .pick_folder()
        .await
        .map(|f| f.path().to_string_lossy().into_owned()))
}
#[tauri::command]
async fn preview(
    selected: Vec<String>,
    values: std::collections::BTreeMap<String, String>,
    state: State<'_, Mutex<Engine>>,
) -> Result<engine::PlanView, String> {
    state
        .lock()
        .map_err(|_| "Manager busy")?
        .preview(selected, values)
}
#[tauri::command]
async fn apply(token: String, state: State<'_, Mutex<Engine>>) -> Result<String, String> {
    state.lock().map_err(|_| "Manager busy")?.apply(&token)
}
#[tauri::command]
async fn restore(state: State<'_, Mutex<Engine>>) -> Result<String, String> {
    state.lock().map_err(|_| "Manager busy")?.restore_latest()
}
#[tauri::command]
async fn import_profile(
    state: State<'_, Mutex<Engine>>,
) -> Result<Option<std::collections::BTreeMap<String, String>>, String> {
    let Some(file) = rfd::AsyncFileDialog::new()
        .add_filter("DS2 settings", &["json"])
        .pick_file()
        .await
    else {
        return Ok(None);
    };
    let raw = platform::read(file.path())?.ok_or("Profile file not found")?;
    if raw.len() > 1024 * 1024 {
        return Err("Profile is too large".into());
    }
    let p: model::Profile = serde_json::from_slice(&raw).map_err(platform::err)?;
    let engine = state.lock().map_err(|_| "Manager busy")?;
    if p.schema_version != 1 {
        return Err("Unknown profile version".into());
    }
    let mut values = engine.data.defaults();
    let mut seen = std::collections::BTreeSet::new();
    for v in p.values {
        let f = engine
            .data
            .fields
            .iter()
            .find(|f| f.id.eq_ignore_ascii_case(&v.id))
            .ok_or("Unknown setting in profile")?;
        if !seen.insert(f.id.clone()) {
            return Err("Duplicate setting in profile".into());
        }
        values.insert(f.id.clone(), f.normalize(&v.value)?);
    }
    Ok(Some(engine.data.validate_values(&values)?))
}
#[tauri::command]
async fn export_profile(
    values: std::collections::BTreeMap<String, String>,
    state: State<'_, Mutex<Engine>>,
) -> Result<bool, String> {
    let values = state
        .lock()
        .map_err(|_| "Manager busy")?
        .data
        .validate_values(&values)?;
    let profile = model::Profile {
        schema_version: 1,
        values: values
            .into_iter()
            .map(|(id, value)| model::Setting { id, value })
            .collect(),
    };
    let Some(file) = rfd::AsyncFileDialog::new()
        .add_filter("DS2 settings", &["json"])
        .set_file_name("DS2-mod-settings.json")
        .save_file()
        .await
    else {
        return Ok(false);
    };
    platform::atomic_write(
        file.path(),
        &serde_json::to_vec_pretty(&profile).map_err(platform::err)?,
    )?;
    Ok(true)
}
fn main() {
    let mut engine = Engine::new();
    let args: Vec<String> = std::env::args().collect();
    if args.len() == 3 && ["--verify-package", "--inspect-game"].contains(&args[1].as_str()) {
        let result = if args[1] == "--verify-package" {
            engine.verify_package()
        } else {
            engine
                .inspect(None)
                .and_then(|v| serde_json::to_value(v).map_err(platform::err))
        };
        let ok = result.is_ok();
        let report = result.unwrap_or_else(|e| serde_json::json!({"ok":false,"error":e}));
        if std::fs::write(&args[2], serde_json::to_vec_pretty(&report).unwrap()).is_err() {
            std::process::exit(2);
        }
        std::process::exit(if ok { 0 } else { 1 });
    }
    tauri::Builder::default()
        .manage(Mutex::new(engine))
        .invoke_handler(tauri::generate_handler![
            inspect,
            choose_game,
            preview,
            apply,
            restore,
            import_profile,
            export_profile
        ])
        .on_window_event(|window, event| {
            if let tauri::WindowEvent::CloseRequested { api, .. } = event {
                if window.state::<Mutex<Engine>>().try_lock().is_err() {
                    api.prevent_close();
                }
            }
        })
        .run(tauri::generate_context!())
        .expect("Unable to start DS2 Mod Suite");
}
