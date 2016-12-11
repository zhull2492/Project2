// Reducer for indexing
// Based on the sample provided

import java.util.*;
import java.io.*;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class GlobalIndexReducer extends Reducer<Text, Text, Text, Text>{
    
    @Override
    public void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException{
        String total = "";
        
        for (Text val : values){
            total += "("+val.toString() +") ";
        }

        context.write(key, new Text(total));
        
    }
    

} 
