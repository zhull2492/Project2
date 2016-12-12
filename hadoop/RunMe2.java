// this periodically updates the golbal index
//
// for tiny google

import java.io.*;
import java.util.*;

public class RunMe2{
    
    // the update time in seconds
    public static final int UPDATE_FREQ = 60 * 2; 

    public static final String inDir  = "test/mv";
    public static final String outDir = "test/out";
    public static final String afsDir = "globalIndex.txt";
    
    public static void main(String[] args){
        
        while(true){
            try{
                System.out.println("[DEBUG] going to sleep");
                Thread.sleep(UPDATE_FREQ * 1000);
            } catch (Exception e){
                System.out.println("[WARN] sleep interupted");
            }
            System.out.println("[DEBUG] awake");

            try{
                // b/c mapReduce doesn't like it when our outdir exists
                Indexer.rmDirFromHDFS(outDir);
            } catch (Exception rmExcept){
                System.out.println("[WARN] could not rm " + outDir);
                System.out.println(rmExcept);
            }
            System.out.println("[INFO] ran outDir rm");

            try{
                Indexer.globalIndex(inDir, outDir);
                System.out.println("[INFO] indexed");        
            } catch (Exception globalIndexExcept){
                System.out.println("[ERROR] could not index");
                System.out.println(globalIndexExcept);
            }
            try{
                Indexer.cpToLocal(outDir + "/part-r-00000", afsDir);
                System.out.println("[INFO] file copied to AFS " + afsDir);
            } catch (Exception afsCp){
                System.out.println("[ERROR] could not cp to afs");
                System.out.println(afsCp);
            }
        }

    }


}
