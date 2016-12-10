CS 2510 Project 2

Hadoop/MapReduce implementation of tiny-google

The idea for this goes:

1) save the user's file to AFS
2) copy user's file from AFS to HDFS
3) run a wordcount MapReduce on that file, to generate a single-file count
4) the original file is no longer needed, that file can be removed
5) save this index into a HDFS folder
6) periodically, run a GolbalIndexer MapReduce on these indexes 
7) the end result should be a global index of files and the word counts.  
8) now import this file into an easy to search data-structure, and run queries against that, periodically updating.  

