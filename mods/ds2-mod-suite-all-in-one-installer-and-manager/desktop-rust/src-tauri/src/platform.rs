use sha2::{Digest, Sha256};
use std::os::windows::{ffi::OsStrExt, fs::MetadataExt};
use std::{
    fs::{self, File, OpenOptions},
    io::{Read, Write},
    path::{Path, PathBuf},
};
use windows_sys::Win32::{
    Foundation::{CloseHandle, INVALID_HANDLE_VALUE},
    Storage::FileSystem::{MoveFileExW, MOVEFILE_REPLACE_EXISTING, MOVEFILE_WRITE_THROUGH},
    System::Diagnostics::ToolHelp::{
        CreateToolhelp32Snapshot, Process32FirstW, Process32NextW, PROCESSENTRY32W,
        TH32CS_SNAPPROCESS,
    },
};
pub fn err(e: impl std::fmt::Display) -> String {
    e.to_string()
}
pub fn id() -> String {
    format!(
        "{}-{}",
        std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_nanos(),
        std::process::id()
    )
}
pub fn hash(data: &[u8]) -> String {
    format!("{:X}", Sha256::digest(data))
}
pub fn hash_file(path: &Path) -> Result<String, String> {
    check_path(path)?;
    let mut f = File::open(path).map_err(err)?;
    let mut h = Sha256::new();
    let mut block = [0u8; 65536];
    loop {
        let n = f.read(&mut block).map_err(err)?;
        if n == 0 {
            break;
        }
        h.update(&block[..n]);
    }
    Ok(format!("{:X}", h.finalize()))
}
pub fn safe_name(name: &str) -> Result<(), String> {
    let head = name.split('.').next().unwrap_or("").to_ascii_lowercase();
    if name.is_empty()
        || name.len() > 160
        || name.starts_with('.')
        || name.ends_with(['.', ' '])
        || !name
            .chars()
            .all(|c| c.is_ascii_alphanumeric() || "_. -".contains(c))
        || ["con", "prn", "aux", "nul"].contains(&head.as_str())
        || ((head.starts_with("com") || head.starts_with("lpt"))
            && head.len() == 4
            && head.as_bytes()[3].is_ascii_digit())
    {
        return Err(format!("Unsafe file name: {}", name));
    }
    Ok(())
}
pub fn check_path(path: &Path) -> Result<(), String> {
    if !path.is_absolute() {
        return Err("Expected absolute path".into());
    }
    let mut current = Some(path);
    while let Some(p) = current {
        match fs::symlink_metadata(p) {
            Ok(m) => {
                if m.file_attributes() & 0x400 != 0 {
                    return Err(format!(
                        "Reparse point or symbolic link rejected: {}",
                        p.display()
                    ));
                }
            }
            Err(e) if e.kind() == std::io::ErrorKind::NotFound => {}
            Err(e) => return Err(err(e)),
        }
        current = p.parent();
    }
    Ok(())
}
pub fn read(path: &Path) -> Result<Option<Vec<u8>>, String> {
    check_path(path)?;
    match fs::metadata(path) {
        Err(e) if e.kind() == std::io::ErrorKind::NotFound => return Ok(None),
        Err(e) => return Err(err(e)),
        Ok(m) => {
            if !m.is_file() || m.len() > 128 * 1024 * 1024 {
                return Err(format!("Unexpected mod file: {}", path.display()));
            }
        }
    }
    fs::read(path).map(Some).map_err(err)
}
pub fn atomic_write(path: &Path, data: &[u8]) -> Result<(), String> {
    check_path(path)?;
    let parent = path.parent().ok_or("Missing parent")?;
    let temp = parent.join(format!("DS2Suite-pending-{}.tmp", id()));
    let result = (|| {
        let mut f = OpenOptions::new()
            .write(true)
            .create_new(true)
            .open(&temp)
            .map_err(err)?;
        f.write_all(data).map_err(err)?;
        f.sync_all().map_err(err)?;
        drop(f);
        check_path(path)?;
        let a: Vec<u16> = temp.as_os_str().encode_wide().chain(Some(0)).collect();
        let b: Vec<u16> = path.as_os_str().encode_wide().chain(Some(0)).collect();
        // Replace a completed file atomically; no shell, injection or executable launch.
        if unsafe {
            MoveFileExW(
                a.as_ptr(),
                b.as_ptr(),
                MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH,
            )
        } == 0
        {
            return Err(err(std::io::Error::last_os_error()));
        }
        Ok(())
    })();
    if result.is_err() {
        let _ = fs::remove_file(&temp);
    }
    result
}
pub fn set_file(path: &Path, data: Option<&[u8]>) -> Result<(), String> {
    check_path(path)?;
    match data {
        Some(d) => atomic_write(path, d),
        None => fs::remove_file(path).map_err(err),
    }
}
pub fn game_running() -> Result<bool, String> {
    // Read-only process-name enumeration. No process memory access or process handles.
    unsafe {
        let snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if snapshot == INVALID_HANDLE_VALUE {
            return Err(err(std::io::Error::last_os_error()));
        }
        let mut entry: PROCESSENTRY32W = std::mem::zeroed();
        entry.dwSize = std::mem::size_of::<PROCESSENTRY32W>() as u32;
        let mut ok = Process32FirstW(snapshot, &mut entry);
        let mut found = false;
        while ok != 0 {
            let len = entry
                .szExeFile
                .iter()
                .position(|c| *c == 0)
                .unwrap_or(entry.szExeFile.len());
            if String::from_utf16_lossy(&entry.szExeFile[..len]).eq_ignore_ascii_case("DS2.exe") {
                found = true;
                break;
            }
            ok = Process32NextW(snapshot, &mut entry);
        }
        CloseHandle(snapshot);
        Ok(found)
    }
}
pub fn user_dir() -> PathBuf {
    PathBuf::from(std::env::var_os("LOCALAPPDATA").unwrap_or_default())
        .join("SimonMods")
        .join("DS2ModSuiteRust")
}
pub fn legacy_dir() -> PathBuf {
    PathBuf::from(std::env::var_os("LOCALAPPDATA").unwrap_or_default())
        .join("SimonMods")
        .join("DS2ModSuite")
}
pub fn discover(steam_folder: &str) -> Vec<PathBuf> {
    let mut candidates = Vec::new();
    for folder in [user_dir(), legacy_dir()] {
        if let Ok(Some(raw)) = read(&folder.join("settings.json")) {
            if let Ok(v) = serde_json::from_slice::<serde_json::Value>(&raw) {
                for key in ["gamePath", "GamePath", "lastGamePath"] {
                    if let Some(s) = v.get(key).and_then(|v| v.as_str()) {
                        candidates.push(PathBuf::from(s));
                    }
                }
            }
        }
    }
    let mut libraries = Vec::new();
    use winreg::{enums::*, RegKey};
    for (hive, key, value) in [
        (HKEY_CURRENT_USER, "Software\\Valve\\Steam", "SteamPath"),
        (
            HKEY_LOCAL_MACHINE,
            "SOFTWARE\\WOW6432Node\\Valve\\Steam",
            "InstallPath",
        ),
    ] {
        if let Ok(key) = RegKey::predef(hive).open_subkey(key) {
            if let Ok(path) = key.get_value::<String, _>(value) {
                libraries.push(PathBuf::from(path));
            }
        }
    }
    for lib in libraries.clone() {
        if let Ok(vdf) = fs::read_to_string(lib.join("steamapps/libraryfolders.vdf")) {
            for line in vdf.lines() {
                let parts: Vec<_> = line.split('"').collect();
                if parts.len() >= 4 && parts[1] == "path" {
                    libraries.push(PathBuf::from(parts[3].replace("\\\\", "\\")));
                }
            }
        }
    }
    for letter in 'C'..='Z' {
        libraries.push(PathBuf::from(format!("{}:\\SteamLibrary", letter)));
        libraries.push(PathBuf::from(format!("{}:\\Steam", letter)));
    }
    candidates.extend(
        libraries
            .into_iter()
            .map(|p| p.join("steamapps/common").join(steam_folder)),
    );
    candidates.retain(|p| p.join("DS2.exe").is_file());
    let mut seen = std::collections::BTreeSet::new();
    candidates.retain(|p| seen.insert(p.to_string_lossy().to_ascii_lowercase()));
    candidates
}
