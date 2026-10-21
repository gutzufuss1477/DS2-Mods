#pragma once
// Seconds configuration for the existing Sam Overhaul road-assist timer.
// Pure parsing code, shared with tests; no locale or CRT dependency.
namespace sam_autodrive {
struct Settings {
    float seconds;
    float multiplier;
    const char* source;
    bool invalid;
    bool enabled;
};
inline bool Space(wchar_t c) { return c==L' ' || c==L'\t' || c==L'\r' || c==L'\n'; }
inline bool ParseSeconds(const wchar_t* text, float& seconds) {
    if (!text) return false;
    const wchar_t* p=text;
    while (Space(*p)) ++p;
    if (*p==L'+') ++p;
    unsigned int whole=0, fraction=0, divisor=1, digits=0;
    bool any=false;
    while (*p>=L'0' && *p<=L'9') {
        if (++digits>16u) return false;
        whole=whole*10u+(unsigned int)(*p++-L'0');
        if (whole>5u) return false;
        any=true;
    }
    if (*p==L'.' || *p==L',') {
        ++p;
        unsigned int fractionalDigits=0;
        while (*p>=L'0' && *p<=L'9') {
            if (++fractionalDigits>6u) return false;
            fraction=fraction*10u+(unsigned int)(*p++-L'0');
            divisor*=10u;
            any=true;
        }
    }
    while (Space(*p)) ++p;
    if (!any || (*p && *p!=L';' && *p!=L'#')) return false;
    const unsigned int micros=whole*1000000u+fraction*(1000000u/divisor);
    if (micros<500000u || micros>5000000u) return false;
    seconds=(float)micros/1000000.0f;
    return true;
}
inline Settings ResolveSettings(const wchar_t* secondsText, bool secondsPresent,
                                unsigned int legacy, bool legacyPresent) {
    Settings result={2.0f,2.5f,"DEFAULT_SECONDS",false,true};
    if (secondsPresent) {
        result.source="SECONDS";
        if (!ParseSeconds(secondsText,result.seconds)) {
            result.seconds=5.0f; result.source="INVALID_SECONDS_VANILLA";
            result.invalid=true;
        }
    } else if (legacyPresent) {
        result.source="LEGACY_MULTIPLIER";
        if (legacy<1u || legacy>10u) {
            result.seconds=5.0f; result.source="INVALID_LEGACY_VANILLA";
            result.invalid=true;
        } else {
            result.seconds=5.0f/(float)legacy;
        }
    }
    result.multiplier=5.0f/result.seconds;
    result.enabled=result.seconds<5.0f;
    return result;
}
} // namespace sam_autodrive
