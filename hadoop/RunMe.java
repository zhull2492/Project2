// the runner for the hadoop part of the tiny-google
//
// if this file looks scary, it's mostly just the error handling 


import java.io.*;
import java.util.*;

public class RunMe {

    private static final String index_loc = "test/mv";
    private static final String tempIn    = "test/tempIn";
    private static final String tempOut   = "test/tempOut";

    public static void main(String[] args){

        // for now, let's just ask the user for the loc, but the file transfer will send it to us over network
        System.out.println("=========================================================");
        System.out.println("   _____  _____ ___  _____ __  ___     ");
        System.out.println("  / ____|/ ____|__ \\| ____/_ |/ _ \\    ");
        System.out.println(" | |    | (___    ) | |__  | | | | |   ");
        System.out.println(" | |     \\___ \\  / /|___ \\ | | | | |   ");
        System.out.println(" | |____ ____) |/ /_ ___) || | |_| |   ");
        System.out.println("  \\_____|_____/|____|____/ |_|\\___/ ");
        System.out.println("=========================================================");

        System.out.println("enter file path:");
        Scanner scan = new Scanner(System.in);

        while(true){
            // the file we're indexing
            String afsLoc = scan.nextLine();
            System.out.println("Great!  Now enter it's identifier:");
            String toAppend = scan.nextLine();
            System.out.println("Awesome! let's get this thing going");

            // let's make tempIn
            try{
                Indexer.mkDirHDFS(tempIn);
            } catch (Exception mkDirExcept){
                // couldn't make the dir
                System.out.println("[ERROR] could not make dir "+tempIn);
                System.out.println(mkDirExcept);
                System.out.println("[INFO] Trying to rm the dir from HDFS . . .");
                try{
                    // let's try rm and mk again
                    Indexer.rmDirFromHDFS(tempIn);
                    Indexer.mkDirHDFS(tempIn);
                } catch (Exception rmmkExcept){
                    // nope, I don't think we can fix this
                    System.out.println("[FATAL] could not recover from error");
                    System.out.println(rmmkExcept);
                    System.exit(-1);
                }

            }
            System.out.println("[INFO] created " + tempIn + " in hdfs");

            // copy afsLoc to tempIn
            try{
                Indexer.copyFromAFS(afsLoc,tempIn);
            } catch (Exception copyAFSExc){
                // couldn't copy, maybe the user gave us a bad file
                System.out.println("[ERROR] could not copy the specified string");
                System.out.println(copyAFSExc);
                System.out.println("try again with a different file");
                continue;
            }
            System.out.println("[INFO] copied file into HDFS");


            // let's index!
            try{ 
                Indexer.indexFile(tempIn, tempOut);
            } catch (Exception indexExcept){
                // indexFile threw us an exception
                System.out.println("[ERROR] could not index file");
                System.out.println(indexExcept);
                System.out.println("[INFO] cleaning up");
                // can't trust this data anymore, let's just clean it up
                try {
                    // let's clean up the tempIn, so we can use it again later
                    Indexer.rmDirFromHDFS(tempIn);
                    System.out.println("[INFO] " + tempIn + " rm'd");
                } catch (Exception tempInCleanExcept){
                    // couldn't clean up, IDK what's going on, let's just stop here
                    System.out.println("[FATAL] could not clean up " + tempIn);
                    System.out.println(tempInCleanExcept);
                    System.exit(-1);
                }
                try{
                    // let's clean up the tempOut 
                    Indexer.rmDirFromHDFS(tempOut);
                    System.out.println("[INFO] " + tempOut + " rm'd");
                    continue;
                } catch (Exception outCleanExcept){
                    // I don't know if this was even here, so if rm fails, we can keep trying
                    System.out.println("[INFO] could not clean " + tempOut);
                    continue;
                }
            }
            System.out.println("[INFO] indexing complete, saved temp to "+ tempOut);


            // rm tempIn
            try{
                Indexer.rmDirFromHDFS(tempIn);
            } catch (Exception tempInRmExcept){
                System.out.println("[ERROR] could not rm " + tempIn);
                System.out.println(tempInRmExcept);
                System.out.println("[TODO] handle this");
            }
            System.out.println("[INFO] " + tempIn + "rm'd");


            // ok, append
            try{
                Indexer.appendStringToHDFS(tempOut + "/part-r-00000", toAppend);
            } catch (Exception appendExcept){
                System.out.println("[ERROR] could not append file with " + toAppend);
                System.out.println(appendExcept);
                System.out.println("[INFO] cleaning up");

                // clean up
                System.out.println("[TODO] clean up here");
                continue;
            }
            System.out.println("[INFO] file appened");


            // Move to final location

            Random generator = new Random(); 
            int i = generator.nextInt(10000000) + 1;
            String UID = (new Integer(i)).toString();

            try{
                Indexer.mvHDFS(tempOut + "/part-r-00000" , index_loc + "/"+UID);
            } catch (Exception finalMvExcept){
                System.out.println("[ERROR] could not move from " + tempOut + " to " + index_loc);
                System.out.println(finalMvExcept);
                System.out.println("[TODO] handle this error");
            }
            System.out.println("[INFO] file moved to " + index_loc + "/" + UID);


            // rm tempOut
            try{
                Indexer.rmDirFromHDFS(tempOut);
            } catch (Exception finalRmExcept){
                System.out.println("[ERROR] could not rm " + tempOut);
                System.out.println(finalRmExcept);
                System.out.println("[TODO] handle this error");
            }
            System.out.println("[INFO] " + tempOut + "rm'd");

            // let's go again!
            System.out.println("enter next file loc");
        }

    }


}
