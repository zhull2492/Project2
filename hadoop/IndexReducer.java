// Reducer for indexing
// Based on the sample provided

import java.util.*;
import java.io.*;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class IndexReducer extends Reducer<Text, IntWritable, Text, IntWritable>{
    
    @Override
    public void reduce(Text key, Iterable<IntWritable> values, Context context) throws IOException, InterruptedException{
        int sum = 0;
        
        for (IntWritable val : values){
            sum += val.get();
        }

        context.write(key, new IntWritable(sum));
        
    }
    

} 
