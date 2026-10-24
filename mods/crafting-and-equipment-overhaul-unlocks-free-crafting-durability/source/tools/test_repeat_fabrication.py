"""Run production special-vehicle callbacks with fake native functions/memory."""
from pathlib import Path
import subprocess
R=Path(__file__).resolve().parents[1]
cpp=R/'tests/coffin_fabrication_runtime_tests.cpp'
exe=R/'build/coffin_fabrication_runtime_tests.exe'
subprocess.run([r'C:\Program Files\LLVM\bin\clang-cl.exe','/nologo','/std:c++17','/EHsc','/MT','/O2',str(cpp),'/Fe'+str(exe),'/link','kernel32.lib'],cwd=R,check=True)
subprocess.run([str(exe)],cwd=R,check=True)
