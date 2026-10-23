#pragma once
// DO NOT replace the whole 11-component Story EntityResource in the native
// baggage viewer. That viewer requires an extra component which is absent
// after the Story swap; Ghidra MCP showed a nullptr write at DS2 RVA 11B540F.
// The authentic Story geometry can ONLY be used by the specific native
// rideable Coffinboard's graphical ArtParts ModelPartResource.
namespace coffin_view_scope {
constexpr bool preserveNativeViewer=true;
constexpr bool permitStoryEntityForBaggageView(bool,bool,bool) {
 return !preserveNativeViewer;
}
}
