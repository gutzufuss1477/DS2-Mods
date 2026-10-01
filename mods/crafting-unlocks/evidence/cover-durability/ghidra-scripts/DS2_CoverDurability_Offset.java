// @category DS2.Crafting
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.io.*;
import java.nio.charset.StandardCharsets;

public class DS2_CoverDurability_Offset extends GhidraScript {
  public void run() throws Exception {
    String[] a=getScriptArgs();
    File out=new File(a[0]);
    PrintWriter p=new PrintWriter(out, StandardCharsets.UTF_8);
    Address base=currentProgram.getImageBase();
    Listing listing=currentProgram.getListing();
    FunctionManager fm=currentProgram.getFunctionManager();
    InstructionIterator it=listing.getInstructions(true);
    while(it.hasNext()){
      Instruction ins=it.next();
      boolean hit=false;
      for(int op=0;op<ins.getNumOperands()&&!hit;op++){
        for(Object o:ins.getOpObjects(op)){
          if(o instanceof Scalar){
            long v=((Scalar)o).getUnsignedValue();
            if(v==0x44A8L || v==0x44A4L || v==0x44A0L) { hit=true; break; }
          }
        }
      }
      if(hit){
        Function f=fm.getFunctionContaining(ins.getAddress());
        p.printf("HIT 0x%X %s %s%n",ins.getAddress().subtract(base),f==null?"<no-func>":f.getName(),ins);
        Instruction x=ins;
        for(int i=0;i<6;i++){ x=x.getPrevious(); if(x==null) break; p.printf("  - 0x%X %s%n",x.getAddress().subtract(base),x); }
        x=ins;
        for(int i=0;i<8;i++){ x=x.getNext(); if(x==null) break; p.printf("  + 0x%X %s%n",x.getAddress().subtract(base),x); }
      }
    }
    p.close();
  }
}