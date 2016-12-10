// a test for some map-reducing!

import java.util.*;
import java.io.*;

public class Index1Test{

    public static void main(String[] args){
        if(args.length != 2){
            System.out.println("usage: Index1Test <input path> <output path>");
            System.exit(-1);
        }

        try{
            Indexer.indexFile(args[0], args[1]);
        } catch(Exception e){
            System.out.println("error " + e);
        }

    }


}
