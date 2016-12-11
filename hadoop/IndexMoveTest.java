// a test for some map-reducing!

import java.util.*;
import java.io.*;

public class IndexMoveTest{

    public static void main(String[] args){
        if(args.length != 4){
            System.out.println("usage: Index1Test <input path> <output path> <renamed> <final res dir>");
            System.exit(-1);
        }

        try{
            Indexer.indexFile(args[0], args[1]);
            Indexer.appendStringToHDFS(args[1]+"/part-r-00000",args[2]);
            Indexer.mvHDFS(args[1]+"/part-r-00000", "test/mv/"+args[2]);
            Indexer.rmDirFromHDFS(args[1]);
            Indexer.globalIndex("test/mv", args[3]);
        } catch(Exception e){
            System.out.println("error " + e);
        }

    }


}
