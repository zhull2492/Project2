// The controller for the various Map/reduce going on
//
// Made for CS2510 Project2

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.fs.FileUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;

import java.io.*;
import java.util.*;

public class Indexer{


    public static void indexFile(String inputPath, String outputPath) throws Exception{
        
        //create the job
        Job job = new Job();
        job.setJarByClass(Indexer.class);
        job.setJobName("index_1_file");
        
        // set the paths
        FileInputFormat.addInputPath(job, new Path(inputPath));
        FileOutputFormat.setOutputPath(job, new Path(outputPath));

        // set out mapper and reducer
        job.setMapperClass(IndexMapper.class);
        job.setCombinerClass(IndexReducer.class);
        job.setReducerClass(IndexReducer.class);

        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        job.waitForCompletion(true);
    }



    public static void copyFromAFS(String AFSdir, String HDFSdir) throws Exception{
        System.out.println("copyFromAFS not yet implemented");
    }


    public static void rmFromHDFS(String HDFSdir) throws Exception{
        System.out.println("rmFromHDFS not yet implemented");
    }


    public static void mvHDFS(String source, String dest) throws Exception{
        Configuration conf = new Configuration();
        FileUtil.copy(FileSystem.get(conf), new Path(source), FileSystem.get(conf), new Path(dest), true, conf);
    }


    public static void globalIndex(String inputPath, String outputPath) throws Exception{
        System.out.println("globalIndex not yet implemented");
    }


}
