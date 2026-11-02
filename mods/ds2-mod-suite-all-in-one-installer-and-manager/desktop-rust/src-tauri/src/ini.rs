use crate::model::Field;
pub struct Ini {
    lines: Vec<String>,
    newline: &'static str,
    bom: bool,
}
impl Ini {
    pub fn parse(text: &str) -> Self {
        let newline = if text.contains("\r\n") { "\r\n" } else { "\n" };
        let text2 = text
            .trim_start_matches('\u{feff}')
            .replace("\r\n", "\n")
            .replace('\r', "\n");
        let mut lines: Vec<String> = text2.split('\n').map(str::to_string).collect();
        if lines.last().is_some_and(|s| s.is_empty()) {
            lines.pop();
        }
        Self {
            lines,
            newline,
            bom: text.starts_with('\u{feff}'),
        }
    }
    fn entries(&self) -> Vec<(usize, String, String, String)> {
        let mut out = Vec::new();
        let mut section = String::new();
        for (i, raw) in self.lines.iter().enumerate() {
            let t = raw.trim();
            if t.starts_with('[') && t.ends_with(']') {
                section = t[1..t.len() - 1].trim().into();
            } else if !t.starts_with(';') && !t.starts_with('#') {
                if let Some((k, v)) = raw.split_once('=') {
                    out.push((
                        i,
                        section.clone(),
                        k.trim().into(),
                        v.split(';').next().unwrap_or("").trim().into(),
                    ));
                }
            }
        }
        out
    }
    pub fn get(&self, field: &Field) -> Option<String> {
        let entries = self.entries();
        for key in std::iter::once(&field.key).chain(field.aliases.iter()) {
            if let Some(e) = entries
                .iter()
                .find(|e| e.1.eq_ignore_ascii_case(&field.section) && e.2.eq_ignore_ascii_case(key))
            {
                return Some(e.3.clone());
            }
        }
        None
    }
    pub fn set(&mut self, field: &Field, value: &str) {
        let entries: Vec<_> = self
            .entries()
            .into_iter()
            .filter(|e| {
                e.1.eq_ignore_ascii_case(&field.section)
                    && (e.2.eq_ignore_ascii_case(&field.key)
                        || field.aliases.iter().any(|a| a.eq_ignore_ascii_case(&e.2)))
            })
            .collect();
        if let Some(first) = entries.first() {
            let raw = &self.lines[first.0];
            let comment = raw
                .find(';')
                .map(|i| format!(" {}", &raw[i..]))
                .unwrap_or_default();
            self.lines[first.0] = format!("{}={}{}", field.key, value, comment);
            for e in entries.iter().skip(1).rev() {
                self.lines.remove(e.0);
            }
        } else {
            let marker = format!("[{}]", field.section);
            if let Some(start) = self
                .lines
                .iter()
                .position(|s| s.trim().eq_ignore_ascii_case(&marker))
            {
                let end = self
                    .lines
                    .iter()
                    .enumerate()
                    .skip(start + 1)
                    .find(|(_, s)| s.trim().starts_with('['))
                    .map(|(i, _)| i)
                    .unwrap_or(self.lines.len());
                self.lines.insert(end, format!("{}={}", field.key, value));
            } else {
                self.lines
                    .extend([String::new(), marker, format!("{}={}", field.key, value)]);
            }
        }
    }
    pub fn render(&self) -> String {
        format!(
            "{}{}{}",
            if self.bom { "\u{feff}" } else { "" },
            self.lines.join(self.newline),
            self.newline
        )
    }
}
