fn main() {
    println!("cargo:rerun-if-changed=app.manifest");
    let attributes = if std::env::var("PROFILE").as_deref() == Ok("release") {
        tauri_build::Attributes::new().windows_attributes(
            tauri_build::WindowsAttributes::new().app_manifest(include_str!("app.manifest")),
        )
    } else {
        // Unit tests and development builds work solely in writable fixtures.
        tauri_build::Attributes::new()
    };
    tauri_build::try_build(attributes).expect("Unable to build Windows resources");
}
