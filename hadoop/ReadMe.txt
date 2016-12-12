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

===================================================================================
compile the *.java files
 * javac -classpath /usr/share/hadoop/hadoop-core-1.0.1.jar *.java
 
 create the Indexer.jar
  * jar -cvf Indexer.jar *.class

create hadoop folders
 * hadoop fs -mkdir test
 * hadoop fs -mkdir test/mv

start the global indexer
 * hadoop jar Indexer.jar RunMe2

start the single file indexer
 * hadoop jar Indexer.jar RunMe

running RunMe:
 * enter a path of a text file
 * enter that file's identifier (path, UID, etc)
 * wait . . .
 * repeat 
 
 running RunMe2:
  * RunMe2 requires no input

results:
 * hadoop fs -cat test/out/part-r-00000

