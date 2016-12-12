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
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.mapreduce.lib.input.KeyValueTextInputFormat;

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



    public static void copyFromAFS(String AFSsrc, String HDFSdir) throws Exception{

        Configuration conf = new Configuration();

        FileSystem hdfs = FileSystem.get(conf);
        hdfs.copyFromLocalFile(new Path(AFSsrc), new Path(HDFSdir));

    }
    

    public static void mkDirHDFS(String HDFSpath) throws Exception{
        Configuration conf = new Configuration();
        FileSystem hdfs = FileSystem.get(conf);
        hdfs.mkdirs(new Path(HDFSpath));

    }


    public static void rmDirFromHDFS(String HDFSdir) throws Exception{
        Configuration conf = new Configuration();
        FileSystem.get(conf).delete(new Path(HDFSdir), true);
    }


    public static void mvHDFS(String source, String dest) throws Exception{
        Configuration conf = new Configuration();
        FileUtil.copy(FileSystem.get(conf), new Path(source), FileSystem.get(conf), new Path(dest), true, conf);
    }


    public static void globalIndex(String inputPath, String outputPath) throws Exception{

        Job job = new Job();
        job.setJarByClass(Indexer.class);
        job.setJobName("global_index");

        FileInputFormat.addInputPath(job, new Path(inputPath));
        FileOutputFormat.setOutputPath(job, new Path(outputPath));

        job.setMapperClass(GlobalIndexMapper.class);
        job.setReducerClass(GlobalIndexReducer.class);

        job.setInputFormatClass(KeyValueTextInputFormat.class);

        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);

        job.waitForCompletion(true);

    }

    public static void appendStringToHDFS(String src, String appendString) throws Exception{

        Configuration conf = new Configuration();
        FileSystem hdfs = FileSystem.get(conf);


        Path path = new Path(src);


        FSDataInputStream instream = hdfs.open(path);
        InputStreamReader instreamreader = new InputStreamReader(instream);


        BufferedReader reader = new BufferedReader(instreamreader);


        String line = reader.readLine();
        String file = "";
        while(line != null){
            file += line + " " + appendString + "\n";
            line = reader.readLine();
        }

        //System.out.println(file);
        reader.close();

        FSDataOutputStream outstream = hdfs.create(path, true);
        outstream.writeUTF(file);

        outstream.close();

    }
}
