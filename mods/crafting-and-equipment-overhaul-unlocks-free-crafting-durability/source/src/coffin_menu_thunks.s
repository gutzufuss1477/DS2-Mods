.intel_syntax noprefix
.text
// The verified special-vehicle callsite retains the exact selected row in RBP.
.globl CoffinFabricateVehicleThunk
CoffinFabricateVehicleThunk:
 mov r8, rbp
 jmp CoffinFabricateVehicle
.globl CoffinCargoDetailIconThunk
CoffinCargoDetailIconThunk:
 mov r8, r14
 jmp CoffinCargoIcon
.globl CoffinCargoHudIconThunk
CoffinCargoHudIconThunk:
 mov r8, rsi
 jmp CoffinCargoIcon
.globl CoffinCargoFocusIconThunk
CoffinCargoFocusIconThunk:
 mov r8, qword ptr [rsp + 0x48]
 jmp CoffinCargoIconById
.globl CoffinMenuGridIconThunk
CoffinMenuGridIconThunk:
 mov r8, qword ptr [r15]
 jmp CoffinMenuIcon
.globl CoffinMenuDetailIconThunk
CoffinMenuDetailIconThunk:
 mov r8, qword ptr [rbp - 0x58]
 mov r8, qword ptr [r8]
 jmp CoffinMenuIcon
.globl CoffinMenuBasketIconThunk
CoffinMenuBasketIconThunk:
 mov r8, rdi
 jmp CoffinMenuIcon
.globl CoffinMenuSummaryIconThunk
CoffinMenuSummaryIconThunk:
 mov r8, rbx
 jmp CoffinMenuIcon
.globl CoffinMenuGridNameThunk
CoffinMenuGridNameThunk:
 mov r9, r15
 jmp CoffinMenuName
.globl CoffinMenuDetailNameThunk
CoffinMenuDetailNameThunk:
 mov r9, qword ptr [rbp - 0x58]
 jmp CoffinMenuName
.globl CoffinMenuSelectedNameThunk
CoffinMenuSelectedNameThunk:
 mov r9, qword ptr [rbp - 0x28]
 jmp CoffinMenuName
.globl CoffinMenuSelectedContentNameThunk
CoffinMenuSelectedContentNameThunk:
 mov r9, qword ptr [rbp - 0x28]
 jmp CoffinMenuName
.globl CoffinMenuBasketNameThunk
CoffinMenuBasketNameThunk:
 mov r9, rsi
 jmp CoffinMenuName
.globl CoffinMenuSummaryNameThunk
CoffinMenuSummaryNameThunk:
 mov r8, rbx
 jmp CoffinMenuCopyName
