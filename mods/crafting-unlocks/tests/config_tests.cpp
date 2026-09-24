// Uses the unchanged production parser and selection core from the shipped ASI.
// These are offline policy tests, not simulated execution of the game's story code.
#include "../src/core.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>
using namespace craft;
static unsigned assertions = 0;
static void check(bool ok, const char* why) {
    ++assertions;
    if (!ok) throw std::runtime_error(why);
}
static Settings read_settings(const std::string& text) {
    Settings settings{};
    Error error{};
    if (!parse(text.data(), static_cast<u32>(text.size()), settings, error)) {
        throw std::runtime_error("INI line " + std::to_string(error.line) + ": " +
                                 (error.message ? error.message : "parse failure"));
    }
    return settings;
}
static std::string change(std::string text, const std::string& old_value,
                          const std::string& new_value) {
    const auto position = text.find(old_value);
    check(position != std::string::npos, "replacement exists");
    check(text.find(old_value, position + old_value.size()) == std::string::npos,
          "replacement is unique");
    text.replace(position, old_value.size(), new_value);
    return text;
}
static std::string key_text(u32 value) {
    const char* digits = "0123456789ABCDEF";
    std::string result = "0x00000000";
    for (unsigned i = 0; i < 8; ++i) result[9 - i] = digits[(value >> (i * 4)) & 15];
    return result;
}
static Recipe fixture(u32 index, u32 key) {
    Recipe r{};
    r.instance = {0x141234000ULL, index, 0x12345678};
    r.resource = 0x150000000ULL + index * 0xB0;
    r.baggage = 0x160000000ULL + index * 0x88;
    r.key = key;
    r.usage = 9;  // All, the usage of the reviewed runtime catalogue entries.
    r.valid = 1;
    r.caseType = 1;
    return r;
}
int main(int argc, char** argv) {
    try {
        if (argc != 2) throw std::runtime_error("Usage: config_tests <actual shipped INI>");
        std::ifstream file(argv[1], std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open INI");
        const std::string text{std::istreambuf_iterator<char>(file),
                               std::istreambuf_iterator<char>()};
        const auto shipped = read_settings(text);
        check(shipped.enabled && shipped.all && !shipped.dump, "shipped global defaults");
        check(shipped.count == 90, "90 parsed recipe keys with inline English comments");
        std::vector<Recipe> recipes;
        for (u32 i = 0; i < shipped.count; ++i) {
            check(shipped.rules[i].value == 1, "all explicit recipes start enabled");
            check(selected(shipped, shipped.rules[i].key), "every shipped key selected");
            recipes.push_back(fixture(i, shipped.rules[i].key));
        }
        const auto original = recipes;
        Instance output[MaxMenu]{};
        auto result = build(shipped, recipes.data(), recipes.size(), nullptr, 0, 2, output);
        check(result.ok && result.count == 90 && result.added == 90, "all-default list");
        check(std::memcmp(recipes.data(), original.data(), recipes.size()*sizeof(Recipe)) == 0,
              "default build does not modify masters");
        for (u32 i = 0; i < shipped.count; ++i) {
            const auto key = shipped.rules[i].key;
            auto excluded = read_settings(change(text, key_text(key) + "=1",
                                                 key_text(key) + "=0"));
            check(!selected(excluded, key), "zero disables only early selection");
            result = build(excluded, recipes.data(), recipes.size(), nullptr, 0, 2, output);
            check(result.ok && result.count == 89 && result.added == 89,
                  "one exclusion keeps 89 other early recipes");
            for (u32 j = 0; j < result.count; ++j)
                check((output[j].flags & 0xFFF) != i, "excluded locked recipe absent");
            check(std::memcmp(recipes.data(), original.data(), recipes.size()*sizeof(Recipe)) == 0,
                  "exclusions never clear or write master flags");

            // Supply the recipe as natively unlocked by the game on a later menu open.
            // The mod must keep the native entry, even though its setting is still 0.
            Instance native = recipes[i].instance;
            native.flags |= 0x8000u | 0x20000u;
            const auto nativeBefore = native;
            result = build(excluded, recipes.data(), recipes.size(), &native, 1, 2, output);
            check(result.ok && result.count == 90 && result.added == 89,
                  "native progression restores availability with the same zero override");
            check(std::memcmp(&output[0], &nativeBefore, sizeof(Instance)) == 0,
                  "native flags, type, and padding retained byte-for-byte");
            check(std::memcmp(&native, &nativeBefore, sizeof(Instance)) == 0,
                  "native input entry untouched");
            unsigned occurrences = 0;
            for (u32 j = 0; j < result.count; ++j)
                if ((output[j].flags & 0xFFF) == i) ++occurrences;
            check(occurrences == 1, "native recipe is not duplicated");
            result = build(shipped, recipes.data(), recipes.size(), &native, 1, 2, output);
            check(result.ok && result.count == 90 && result.added == 89,
                  "setting one also avoids duplicate native recipes");
            check(std::memcmp(output, &nativeBefore, sizeof(Instance)) == 0,
                  "setting one also keeps the complete native record");
        }
        auto disabled = read_settings(change(text, "Enabled=1", "Enabled=0"));
        Instance native = recipes[0].instance;
        native.flags |= 0x8000u;
        result = build(disabled, recipes.data(), recipes.size(), &native, 1, 2, output);
        check(result.ok && result.count == 1 && result.added == 0,
              "disabled mod keeps only supplied native entries");
        check(std::memcmp(output, &native, sizeof(Instance)) == 0, "disabled native unchanged");

        check(selected(shipped, 0xFFFFFFFF), "unlisted keys inherit DefaultUnlock=1");
        auto defaultOff = read_settings(change(text, "DefaultUnlock=1", "DefaultUnlock=0"));
        check(!selected(defaultOff, 0xFFFFFFFF), "unlisted key follows changed default");
        check(selected(defaultOff, shipped.rules[0].key), "explicit one overrides global zero");
        auto inherited = read_settings(change(text, key_text(shipped.rules[0].key) + "=1",
                                               key_text(shipped.rules[0].key) + "=inherit"));
        check(selected(inherited, shipped.rules[0].key), "inherit follows all-enabled default");
        inherited = read_settings(change(change(text, key_text(shipped.rules[0].key) + "=1",
                                                key_text(shipped.rules[0].key) + "=inherit"),
                                          "DefaultUnlock=1", "DefaultUnlock=0"));
        check(!selected(inherited, shipped.rules[0].key), "inherit follows all-disabled default");

        const auto diagnostics = read_settings(change(text, "ExportCatalogue=0", "ExportCatalogue=1"));
        check(diagnostics.dump && diagnostics.enabled && diagnostics.all && diagnostics.count==90,
              "diagnostic toggle does not alter selection");
        for (u32 i=0;i<90;++i)
            check(diagnostics.rules[i].key==shipped.rules[i].key && diagnostics.rules[i].value==1,
                  "diagnostic toggle preserves every item rule");
        check(std::memcmp(recipes.data(), original.data(), recipes.size()*sizeof(Recipe)) == 0,
              "masters unchanged at end of all configuration tests");
        std::cout << "PASS " << assertions << " config/progression-policy assertions; 90 shipped recipe keys.\n"
                  << "Original production core used unchanged. Native progression was supplied as test input.\n"
                  << "No Windows/game execution; no claim that all story rewards or notifications were tested.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << assertions << ": " << error.what() << '\n';
        return 1;
    }
}
