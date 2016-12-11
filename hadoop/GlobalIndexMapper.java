// Mapper for an Index generator
// Based on the sample provided

import java.io.IOException;
import java.util.*;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class GlobalIndexMapper extends Mapper<Text, Text, Text, Text>{
    
    
    public void map(Text key, Text value, Context context) throws IOException, InterruptedException{
        
        context.write(key, value);
        
    }
} 
