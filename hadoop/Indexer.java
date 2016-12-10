// The controller for the various Map/reduce going on
//
// Made for CS2510 Project2

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import java.io.*;
import java.util.*;

public class Indexer{


    public static void indexFile(String inputPath, String outputPath) throws Exception{
        System.out.println("indexFile not yet implemented");        
        
    }


    public static void copyFromAFS(String AFSdir, String HDFSdir) throws Exception{
        System.out.println("copyFromAFS not yet implemented");
    }


    public static void rmFromHDFS(String HDFSdir) throws Exception{
        System.out.println("rmFromHDFS not yet implemented");
    }


    public static void mvHDFS(String sourceDir, destDir) throws Exception{
        System.out.println("mv HDFS not yet implemented");
    }


    public static void globalIndex(String inputPath, String outputPath) throws Exception{
        System.out.println("globalIndex not yet implemented");
    }


}
