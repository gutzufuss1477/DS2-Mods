// @category DS2.Crafting
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.*;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class DS2_CoverDurability_Xrefs extends GhidraScript {
  public void run() throws Exception {
    String[] a=getScriptArgs();
    File out=new File(a.length>0?a[0]:"C:\\Users\\Simon\\Downloads\\cover_xrefs.txt");
    PrintWriter p=new PrintWriter(out, StandardCharsets.UTF_8);
    Address base=currentProgram.getImageBase();
    Listing listing=currentProgram.getListing();
    FunctionManager fm=currentProgram.getFunctionManager();
    p.println("IMAGE_BASE="+base);
    p.println("=== INSTRUCTIONS WITH 0x44A8 / 17576 ===");
    InstructionIterator it=listing.getInstructions(true);
    while(it.hasNext()){
      Instruction ins=it.next();
      String s=ins.toString().toLowerCase(Locale.ROOT);
      if(s.contains("44a8") || s.contains("17576")){
        Function f=fm.getFunctionContaining(ins.getAddress());
        long rva=ins.getAddress().subtract(base);
        p.printf("0x%X\t%s\t%s%n",rva,f==null?"<no-func>":f.getName(),ins);
      }
    }
    p.println("=== XREFS TO MANAGER GLOBAL 0x623E4E0 ===");
    Address g=base.add(0x623E4E0L);
    ReferenceIterator ri=currentProgram.getReferenceManager().getReferencesTo(g);
    while(ri.hasNext()){
      Reference r=ri.next();
      Function f=fm.getFunctionContaining(r.getFromAddress());
      p.printf("0x%X\t%s\t%s%n",r.getFromAddress().subtract(base),f==null?"<no-func>":f.getName(),r.getReferenceType());
    }
    p.close();
    println("WROTE "+out.getAbsolutePath());
  }
}