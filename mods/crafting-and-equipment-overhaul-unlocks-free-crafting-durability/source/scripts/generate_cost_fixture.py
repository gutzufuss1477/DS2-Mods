"""Generate standalone reference snippets from the already reviewed cost instructions."""
from pathlib import Path
import json
ROOT=Path(__file__).resolve().parents[1]
records=json.loads((ROOT/'evidence/freecrafting/cost_sites_reviewed.json').read_text())
template=(ROOT/'tests/cost_fixture.s').read_text()
cut=template.index('// Deliberately')
first, helper=template[:cut],template[cut:]
assembly=first.replace('RunCostFixture','RunCostFixture0')
assembly+=first.replace('RunCostFixture','RunCostFixture1').replace('rsp, 40','rsp, 32').replace('.seh_stackalloc 40','.seh_stackalloc 32')
assembly+=helper
registers=['rax','rbx','rcx','rdx','rsi','rdi','rbp','rsp','r8','r9','r10','r11','r12','r13','r14','r15']
header=['#pragma once']
for i,r in enumerate(records):
    name=f'CostReference_{i}'
    header.append(f'extern "C" void {name}();')
    assembly+=f'\n.globl {name}\n{name}:\n.byte '+','.join('0x'+r['original_bytes'][j:j+2] for j in range(0,len(r['original_bytes']),2))+'\nret\n'
header+=['inline craft::CostThunk const CostReferences[]={'+','.join(f'&CostReference_{i}' for i in range(len(records)))+'};']
header+=['constexpr unsigned CostSourceRegister[]={'+','.join(str(registers.index(r['source'])) for r in records)+'};']
(ROOT/'tests/cost_fixture_generated.s').write_text(assembly,encoding='ascii')
(ROOT/'tests/cost_fixture_generated.hpp').write_text('\n'.join(header)+'\n',encoding='ascii')
print('Generated reference code for 44 reviewed instructions and both stack alignments.')
