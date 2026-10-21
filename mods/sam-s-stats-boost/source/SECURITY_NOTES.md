# Security Notes

The ASI is an import-free x64 DLL built from the included C++ and assembly source. It resolves a minimal set of Windows APIs from already loaded modules, validates the exact DS2 executable header, checks unique producer signatures and patch-site bytes, and fails closed on a mismatch.

It does not edit save files, contact the network, launch external processes, or write outside its sibling INI/log paths. All progression changes are committed through DS2's original play-record mutators.
