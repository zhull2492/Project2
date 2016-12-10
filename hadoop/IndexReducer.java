// Reducer for indexing
// Based on the sample provided

import java.util.*;
import java.io.*;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class IndexReducer extends MapReduceBase implements Reducer<Text, IntWritable, Text, IntWritable>{

    public void reduce(Text key, Iterator<IntWritable> values, OutputCollector<Text, IntWritable> output, Reporter reporter) throws IOException{
        int sum = 0;
        
        while (values.hasNext()){
            sum += values.next().get();
        }

        output.collect(key, new IntWritable(sum));
        
    }
    

} 
