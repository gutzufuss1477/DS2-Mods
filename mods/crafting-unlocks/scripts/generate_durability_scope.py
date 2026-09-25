from pathlib import Path
import csv,re

root=Path(__file__).resolve().parents[1]
ini=(root/"release/ds2_crafting_unlocks.ini").read_text(encoding="utf-8-sig")
keys={int(x,16) for x in re.findall(r'^\s*(0x[0-9A-Fa-f]{8})\s*=',ini,re.M)}
rows=list(csv.DictReader((root/"evidence/config-r1/ds2_crafting_catalogue.tsv").open(encoding="utf-8-sig"),delimiter="\t"))
selected=[r for r in rows if int(r["recipe_key"],16) in keys]
assert len(keys)==120 and len(selected)==120
bags=sorted({int(r["bag_namecode"],16) for r in selected})
assert len(bags)==120 and 0 not in bags

boot_recipe_keys=[0x6E7D4315,0x7D2DB0E1,0x0F4633E2,0x1C16C016,0x5B8C5709,0x16CF98D9,0x75D99124]
boot_rows=[next(r for r in selected if int(r["recipe_key"],16)==k) for k in boot_recipe_keys]
boot_bags=sorted({int(r["bag_namecode"],16) for r in boot_rows})
assert len(boot_bags)==7 and 0 not in boot_bags

a=[
".intel_syntax noprefix",".text",
".extern DurabilityMultiplierBits",
".extern DurabilityUnbreakableValue","",
".globl DurabilityContentsThunk",
"DurabilityContentsThunk:",
"    pushfq",
"    push rax",
"    mov rax, qword ptr [rdi + 0x38]",
"    test rax, rax",
"    jz .contents_vanilla",
"    mov eax, dword ptr [rax + 0x44]",
]
for k in bags:
    a += [f"    cmp eax, 0x{k:08X}","    je .contents_supported"]
a += [
"    jmp .contents_vanilla",
".contents_supported:",
"    cmp dword ptr [rip + DurabilityUnbreakableValue], 0",
"    jne .contents_unbreakable",
"    vsubss xmm1, xmm6, xmm1",
"    vdivss xmm1, xmm1, dword ptr [rip + DurabilityMultiplierBits]",
"    vsubss xmm1, xmm6, xmm1",
"    vmaxss xmm1, xmm1, xmm13",
"    jmp .contents_store",
".contents_unbreakable:",
"    vmovaps xmm1, xmm6",
"    jmp .contents_store",
".contents_vanilla:",
"    # Unsupported baggage (normal cargo/order/material/etc.): keep native value.",
".contents_store:",
"    pop rax",
"    popfq",
"    vmovss dword ptr [rdi + 0x88], xmm1",
"    ret",
"",
".globl DurabilityBootsWearThunk",
"DurabilityBootsWearThunk:",
"    # Replays the displaced native load. Flags from VCOMISS at E70C52 are live.",
"    vmovss xmm6, dword ptr [rbp + 0x200]",
"    pushfq",
"    push rax",
"    mov rax, qword ptr [rbx + 0x418]",
"    test rax, rax",
"    jz .boots_done",
"    mov rax, qword ptr [rax + 0x38]",
"    test rax, rax",
"    jz .boots_done",
"    mov eax, dword ptr [rax + 0x44]",
]
for k in boot_bags:
    a += [f"    cmp eax, 0x{k:08X}","    je .boots_supported"]
a += [
"    jmp .boots_done",
".boots_supported:",
"    cmp dword ptr [rip + DurabilityUnbreakableValue], 0",
"    jne .boots_unbreakable",
"    vdivss xmm6, xmm6, dword ptr [rip + DurabilityMultiplierBits]",
"    jmp .boots_done",
".boots_unbreakable:",
"    vxorps xmm6, xmm6, xmm6",
".boots_done:",
"    pop rax",
"    popfq",
"    ret",
]
(root/"src/durability_thunks.s").write_text("\n".join(a)+"\n",encoding="ascii")

h=["#pragma once","namespace craft {","constexpr u32 DurabilityCraftedBaggage[] = {"]
for i in range(0,len(bags),8):
    h.append("  "+",".join(f"0x{x:08X}u" for x in bags[i:i+8])+",")
h += ["};",f"constexpr u32 DurabilityCraftedBaggageCount={len(bags)};","",
      "constexpr u32 DurabilityBootBaggage[] = {"]
for i in range(0,len(boot_bags),8):
    h.append("  "+",".join(f"0x{x:08X}u" for x in boot_bags[i:i+8])+",")
h += ["};",f"constexpr u32 DurabilityBootBaggageCount={len(boot_bags)};","}"]
(root/"src/durability_scope.hpp").write_text("\n".join(h)+"\n",encoding="ascii")
print("Generated durability allowlists:",len(bags),"crafted baggage IDs;",len(boot_bags),"boot baggage IDs")
for r in boot_rows:
    print(r["recipe_key"],r["name"],r["bag_namecode"])
