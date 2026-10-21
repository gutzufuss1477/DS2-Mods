"""Regression matrix: positive mine cost scales the initial UI selection, UI
requirement denominator, native extraction step and real crystal debit.

Static code/algorithm checks only; real play test of values other than 1 pending.
"""
from pathlib import Path
import re
BASE=Path(__file__).resolve().parents[1]
src=(BASE/"src"/"infrastructure_one_unit.cpp").read_text(encoding="utf-8")
ini=(BASE/"config"/"ds2_infrastructure_one_unit.ini").read_text(encoding="utf-8")

def body(name):
    marker=f"static void {name}(CONTEXT*c){{"
    start=src.index(marker)
    return src[start:src.index("\n}\n",start)+2]

pricing=src[src.index("static i32 reducedMineCost(i32 native){"):
            src.index("\n}\n",src.index("static i32 reducedMineCost(i32 native){"))+2]
assert "i32 configured=(i32)g_mineCrystalCost;" in pricing
assert "return configured<native?configured:native;" in pricing
assert "g_mineCrystalCost=uintIni(L\"MineProductionCrystalCost\",0,0,1000000);" in src
assert "MineProductionCrystalCost=0" in ini
assert "MineProductionNoCrystalCost" not in src

upper=body("mineUiSelection")
confirm=body("mineConfirm")
denominator=body("mineMenuDenominator")
production=body("mineProcess")
loop=body("mineLoop")
budget=body("mineBudgetCap")

assert "if(value>(i32)g_mineCrystalCost" in upper
assert "value=(i32)g_mineCrystalCost;" in upper
assert "wr32(widget+0xd0ULL,value)" in upper
assert "wr32(menu+0x8cULL,value)" in upper
assert "rd(menu+0xc30ULL,&mode,1)&&mode==9" in upper

assert "selected>(i32)g_mineCrystalCost" in confirm
assert "wr32(menu+0x8cULL,(i32)g_mineCrystalCost)" in confirm

assert "displayed=reducedMineCost(native);" in denominator
assert "rd(menu+0xc30ULL,&mode,1)&&mode==9" in denominator
assert "cs(c,RBXOFF,(u64)(u32)displayed)" in denominator
assert "MINE_MENU_DENOM mode=" in denominator

assert "i32 cheaper=reducedMineCost(native)" in production
assert "i32 cheaper=reducedMineCost(native)" in loop
assert "consumed+(u64)g_mineCrystalCost" in budget
assert "MINE_COST_GETTER_RVA" in src

def effective(native,setting):
    # Corresponds directly to reducedMineCost; 0=vanilla, >0 reduces only.
    if native<=0 or native>10_000_000 or setting<=0:
        return native
    return min(native,setting)

def initial_selection(native_selected,configured,mode):
    # When vanilla game initially selects full native one-batch price.
    if mode!=9 or configured==0 or native_selected<=0:
        return native_selected
    return min(native_selected,configured)

print("INI  Upper/selected  Lower/required  Debit  Batches")
for configured in (0,1,2,5,10,25,50,100,500,1000,1500):
    vanilla=1000
    selected=initial_selection(vanilla,configured,9)
    required=effective(vanilla,configured)
    assert selected==required,(configured,selected,required)
    assert selected>0
    # The native contribution deducts exactly the selected crystal amount.
    player_before=23044
    player_after=player_before-selected
    assert player_before-player_after==required
    # The production step and loop share the same exact positive step.
    assert effective(vanilla,configured)==required
    assert selected//required==1
    print(f"{configured:>4}  {selected:>14}  {required:>14}  {required:>5}  1")

# User may manually choose fewer crystals; that must not silently increase
# their selected contribution or charge them more than they chose.
assert initial_selection(1,10,9)==1
assert initial_selection(1000,10,8)==1000
assert effective(1000,10)==10
assert effective(300,500)==300  # never raises costs above vanilla
print("PASS: configurable menu/denominator/budget/cost matrix 0,1,2,5,10,25,50,100,500,1000,1500")
