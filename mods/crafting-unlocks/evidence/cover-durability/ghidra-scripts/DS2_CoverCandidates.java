// @category DS2.Crafting
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class DS2_CoverCandidates extends GhidraScript {
  public void run() throws Exception {
    String[] a=getScriptArgs();
    PrintWriter p=new PrintWriter(new File(a[0]), StandardCharsets.UTF_8);
    Address base=currentProgram.getImageBase();
    Listing l=currentProgram.getListing();
    FunctionManager fm=currentProgram.getFunctionManager();
    long[] hits={0xCDD78L,0x5C71C4L,0xB394CEL,0xB3B1D9L,0xB3D25CL,0xB43419L,0xB4342DL,0xB4343CL,0xB434C4L,0x12BF232L,0x151A77EL,0x1528C41L,0x169E6A5L,0x169F590L,0x169FD7FL,0x16AA4E3L,0x16AA636L,0x16AA656L,0x16AA664L,0x16AA673L,0x16AA85EL,0x16AAA4DL,0x16AAA6CL,0x16AAA7AL,0x16AAA88L,0x19B6F92L,0x19BAC23L,0x19BAC2FL,0x19BAC49L,0x19BAC55L};
    Set<Long> done=new HashSet<>();
    for(long h:hits){
      Address x=base.add(h);
      Instruction ins=l.getInstructionContaining(x);
      Function f=ins==null?null:fm.getFunctionContaining(ins.getAddress());
      long fe=f==null?-1:f.getEntryPoint().subtract(base);
      p.printf("\n=== BYTE HIT 0x%X | INS %s | FUNC %s 0x%X ===%n",h,ins==null?"<none>":ins.getAddress().subtract(base),f==null?"<none>":f.getName(),fe);
      if(ins!=null){
        Instruction q=ins;
        for(int i=0;i<10;i++){ q=q.getPrevious(); if(q==null)break; }
        for(int i=0;i<24&&q!=null;i++,q=q.getNext()) p.printf("0x%X\t%s%n",q.getAddress().subtract(base),q);
      }
    }
    p.close();
  }
}