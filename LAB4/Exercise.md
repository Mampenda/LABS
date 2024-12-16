# Exercise 4 - Hadoop

## Task 1

### The Hadoop MapReduce tutorial - WordCount

#### Log into hadoopedge.dat351 remotely and cd into correct folder

```bash
amali@MampendaPC MINGW64 ~/VSCodeProjects/DAT351/LABS (main)
$ ssh -J dat351@eple.hvl.no student1@hadoopedge.dat351
Enter passphrase for key '/c/Users/amali/.ssh/id_rsa':
(dat351@eple.hvl.no) Verification code:
The authenticity of host 'hadoopedge.dat351 (<no hostip for proxy command>)' can't be established.
ED25519 key fingerprint is SHA256:hFXj4YIzFiyf/wGO7cdv8Xd1gyD+7CkySdZGqU8V5C4.
This host key is known by the following other names/addresses:
    ~/.ssh/known_hosts:14: slurmmaster
    ~/.ssh/known_hosts:17: slurmw1
    ~/.ssh/known_hosts:18: arcclient
    ~/.ssh/known_hosts:26: mpi1.dat351
    ~/.ssh/known_hosts:27: mpi1
    ~/.ssh/known_hosts:39: slurmmaster.dat351
Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
Warning: Permanently added 'hadoopedge.dat351' (ED25519) to the list of known hosts.
student1@hadoopedge.dat351's password:
[student1@hadoopedge ~]$ ls -l
total 4
drwxr-xr-x 9 student1 student 92 Nov 19 18:28 dat351
-rw-r--r-- 1 student1 student 64 Aug 30 13:40 me.txt
[student1@hadoopedge ~]$ cd dat351/
[student1@hadoopedge dat351]$ mkdir hadoop
[student1@hadoopedge dat351]$ cd hadoop
```

#### Edit Firefox Network Configurations

1. Open Firefox and go to Edit|Settings|General|Netwrok Settings
2. Check the "Manual proxy-settings" option
3. Put in "localhost" in the SOCKS-server option
4. Enter "7722" as port
5. Check "Proxy DNS when using SOCKS v5"

#### Check User Environement, Create a Directory on HDFS and Verify it

```bash
[student1@hadoopedge ~]$ echo $USER
student1
[student1@hadoopedge ~]$ hdfs dfs -mkdir /user/student1/wordcount
[student1@hadoopedge ~]$ hdfs dfs -ls /user/student1
Found 1 items
drwxr-xr-x   - student1 student1          0 2024-12-16 16:02 /user/student1/wordcount
```

#### Create Input Files for the Program

```bash
[student1@hadoopedge hadoop]$ mkdir -p hadoop-files/wordcount/input/
[student1@hadoopedge hadoop]$ ls -l
total 0
drwxr-xr-x 3 student1 student 22 Dec 16 16:10 hadoop-files
[student1@hadoopedge hadoop]$ ls -l hadoop-files/
total 0
drwxr-xr-x 3 student1 student 18 Dec 16 16:10 wordcount
```

```bash
[student1@hadoopedge hadoop]$ cat > hadoop-files/wordcount/input/file01.txt <<EOF
> Hello World Goodbye World
> EOF
```

```bash
[student1@hadoopedge hadoop]$ cat > hadoop-files/wordcount/input/file02.txt <<EOF
> Hello Hadoop Goodbye Hadoop
> EOF
```

```bash
[student1@hadoopedge hadoop]$ ls -l hadoop-files/wordcount/input/
total 8
-rw-r--r-- 1 student1 student 26 Dec 16 16:13 file01.txt
-rw-r--r-- 1 student1 student 28 Dec 16 16:14 file02.txt
[student1@hadoopedge hadoop]$ cat hadoop-files/wordcount/input/*
Hello World Goodbye World
Hello Hadoop Goodbye Hadoop
[student1@hadoopedge hadoop]$
```

hadoop fs -copyFromLocal /share/home/student1/dat351/hadoop/hadoop-files/wordcount/input/\* /user/student1/wordcount/input/

#### Upload the input files to the HDFS file system (not necessary since Hadoop can stage them when running the program)

```bash
[student1@hadoopedge hadoop]$ hdfs dfs -put hadoop-files/wordcount/input wordcount
[student1@hadoopedge wordcount]$ hdfs dfs -ls /user/student1/wordcount

ls: `/user/student1/wordcount`: No such file or directory
[student1@hadoopedge wordcount]$ hdfs dfs -ls /user/student1
[student1@hadoopedge wordcount]$ hdfs dfs -mkdir -p /user/student1/wordcount/input
[student1@hadoopedge wordcount]$ hdfs dfs -ls /user/student1/wordcount
Found 1 items
drwxr-xr-x   - student1 student1          0 2024-12-16 17:19 /user/student1/wordcount/input
[student1@hadoopedge wordcount]$ hdfs dfs -put input/* /user/student1/wordcount/input/
[student1@hadoopedge wordcount]$ hdfs dfs -ls /user/student1/wordcount/input
Found 2 items
-rw-r--r--   3 student1 student1         26 2024-12-16 17:19 /user/student1/wordcount/input/file01.txt
-rw-r--r--   3 student1 student1         28 2024-12-16 17:19 /user/student1/wordcount/input/file02.txt
[student1@hadoopedge hadoop]$ hdfs dfs -cat "wordcount/input/*"
Hello World Goodbye World
Hello Hadoop Goodbye Hadoop
```

#### Create a Directory the same place as the WordCount Java file and compile it into a JAR

```bash
[student1@hadoopedge wordcount]$ ls -l
total 0
drwxr-xr-x 2 student1 student 40 Dec 16 16:14 input
[student1@hadoopedge wordcount]$ nano WordCount.java
[student1@hadoopedge wordcount]$ ls -l
total 4
drwxr-xr-x 2 student1 student   40 Dec 16 16:14 input
-rw-r--r-- 1 student1 student 2089 Dec 16  2024 WordCount.java
[student1@hadoopedge wordcount]$ cat WordCount.java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class WordCount {

  public static class TokenizerMapper
       extends Mapper<Object, Text, Text, IntWritable>{

    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();

    public void map(Object key, Text value, Context context
                    ) throws IOException, InterruptedException {
      StringTokenizer itr = new StringTokenizer(value.toString());
      while (itr.hasMoreTokens()) {
        word.set(itr.nextToken());
        context.write(word, one);
      }
    }
  }

  public static class IntSumReducer
       extends Reducer<Text,IntWritable,Text,IntWritable> {
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values,
                       Context context
                       ) throws IOException, InterruptedException {
      int sum = 0;
      for (IntWritable val : values) {
        sum += val.get();
      }
      result.set(sum);
      context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "word count");
    job.setJarByClass(WordCount.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}
```

#### Check Java Version and Compile the Java Code With Java 8 or Older (since Hadoop is running on Java 8)

```bash
[student1@hadoopedge wordcount]$ java -version
openjdk version "21.0.5" 2024-10-15 LTS
OpenJDK Runtime Environment (Red_Hat-21.0.5.0.10-1) (build 21.0.5+10-LTS)
OpenJDK 64-Bit Server VM (Red_Hat-21.0.5.0.10-1) (build 21.0.5+10-LTS, mixed mode, sharing)
[student1@hadoopedge wordcount]$ update-alternatives --config java

There are 3 programs which provide 'java'.

  Selection    Command
-----------------------------------------------
*  1           java-1.8.0-openjdk.x86_64 (/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.432.b06-2.el8.x86_64/jre/bin/java)
   2           java-17-openjdk.x86_64 (/usr/lib/jvm/java-17-openjdk-17.0.13.0.11-3.el8.x86_64/bin/java)
 + 3           java-21-openjdk.x86_64 (/usr/lib/jvm/java-21-openjdk-21.0.5.0.10-3.el8.x86_64/bin/java)

Enter to keep the current selection[+], or type selection number:
failed to create /var/lib/alternatives/java.new: Permission denied
[student1@hadoopedge wordcount]$ javac -source 1.8 -target 1.8 -classpath $(hadoop classpath) -d . WordCount.java
warning: [options] bootstrap class path not set in conjunction with -source 8
warning: [options] source value 8 is obsolete and will be removed in a future release
warning: [options] target value 8 is obsolete and will be removed in a future release
warning: [options] To suppress warnings about obsolete options, use -Xlint:-options.
4 warnings
```

#### Create the JAR (using `javac`directly) and Upload it to HDFS

```bash
[student1@hadoopedge wordcount]$ jar -cvf wordcount.jar -C . .
added manifest
adding: WordCount$IntSumReducer.class(in = 1774) (out= 763)(deflated 56%)
adding: WordCount$TokenizerMapper.class(in = 1771) (out= 780)(deflated 55%)
adding: WordCount.class(in = 1485) (out= 819)(deflated 44%)
adding: WordCount.java(in = 2089) (out= 702)(deflated 66%)
adding: input/(in = 0) (out= 0)(stored 0%)
adding: input/file01.txt(in = 26) (out= 23)(deflated 11%)
adding: input/file02.txt(in = 28) (out= 24)(deflated 14%)
[student1@hadoopedge wordcount]$ hdfs dfs -put wordcount.jar /user/student1/wordcount/
```

#### Create the JAR (using `hadoop com.sun.tools.javac.Main`), Create the JAR file, and Upload the JAR to HDFS

```bash
[student1@hadoopedge wordcount]$ hadoop com.sun.tools.javac.Main WordCount.java
[student1@hadoopedge wordcount]$ jar cf wc.jar WordCount*.class
[student1@hadoopedge wordcount]$ hdfs dfs -put wc.jar /user/student1/wordcount/
[student1@hadoopedge wordcount]$ ls -l
total 20
drwxr-xr-x 2 student1 student   40 Dec 16 16:14  input
-rw-r--r-- 1 student1 student 3051 Dec 16 17:09  wc.jar
-rw-r--r-- 1 student1 student 1739 Dec 16 17:09 'WordCount$IntSumReducer.class'
-rw-r--r-- 1 student1 student 1736 Dec 16 17:09 'WordCount$TokenizerMapper.class'
-rw-r--r-- 1 student1 student 1491 Dec 16 17:09  WordCount.class
-rw-r--r-- 1 student1 student 2089 Dec 16 16:28  WordCount.java
```

**PS!** If you've ran the program before, clean the output directory using `hdfs dfs -rm -skipTrash -r wordcount/output`

#### Run the Program

```bash
[student1@hadoopedge wordcount]$ hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/wc.jar WordCount wordcount/input wordcount/output
2024-12-16 17:19:50,904 INFO client.DefaultNoHARMFailoverProxyProvider: Connecting to ResourceManager at hadoopname.dat351/10.0.0.175:8032
2024-12-16 17:19:51,288 WARN mapreduce.JobResourceUploader: Hadoop command-line option parsing not performed. Implement the Tool interface and execute your application with ToolRunner to remedy this.
2024-12-16 17:19:51,303 INFO mapreduce.JobResourceUploader: Disabling Erasure Coding for path: /tmp/hadoop-yarn/staging/student1/.staging/job_1734003739512_0008
2024-12-16 17:19:51,579 INFO input.FileInputFormat: Total input files to process : 2
2024-12-16 17:19:51,668 INFO mapreduce.JobSubmitter: number of splits:2
2024-12-16 17:19:51,841 INFO mapreduce.JobSubmitter: Submitting tokens for job: job_1734003739512_0008
2024-12-16 17:19:51,841 INFO mapreduce.JobSubmitter: Executing with tokens: []
2024-12-16 17:19:52,002 INFO conf.Configuration: found resource resource-types.xml at file:/usr/local/hadoop/3.3.4/etc/hadoop/resource-types.xml
2024-12-16 17:19:52,066 INFO impl.YarnClientImpl: Submitted application application_1734003739512_0008
2024-12-16 17:19:52,112 INFO mapreduce.Job: The url to track the job: http://hadoopname.dat351:8088/proxy/application_1734003739512_0008/
2024-12-16 17:19:52,113 INFO mapreduce.Job: Running job: job_1734003739512_0008
2024-12-16 17:19:59,202 INFO mapreduce.Job: Job job_1734003739512_0008 running in uber mode : false
2024-12-16 17:19:59,204 INFO mapreduce.Job:  map 0% reduce 0%
2024-12-16 17:20:05,342 INFO mapreduce.Job:  map 100% reduce 0%
2024-12-16 17:20:10,380 INFO mapreduce.Job:  map 100% reduce 100%
2024-12-16 17:20:10,393 INFO mapreduce.Job: Job job_1734003739512_0008 completed successfully
...
...
...
```

#### Check the Output

```bash
[student1@hadoopedge wordcount]$ hdfs dfs -ls wordcount/output
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 17:20 wordcount/output/_SUCCESS
-rw-r--r--   3 student1 student1         35 2024-12-16 17:20 wordcount/output/part-r-00000
[student1@hadoopedge wordcount]$ hdfs dfs -cat "wordcount/output/*"
2024-12-16 17:26:43,418 INFO hdfs.DFSClient: Successfully connected to /10.0.0.179:9866 for BP-651351515-10.0.0.175-1695113042748:blk_1073744406_3583
Goodbye 2
Hadoop  2
Hello   2
World   2
[student1@hadoopedge wordcount]$ hdfs dfs -cat "wordcount/input/*"
Hello World Goodbye World
Hello Hadoop Goodbye Hadoop
```

## Task 2

### Using the Tool Interface

#### Copy and Study the Provided Java Program

```bash
[student1@hadoopedge wordcount]$ cp /share/dat351/java/WordCountTools.java .
[student1@hadoopedge wordcount]$ cat WordCountTools.java
```

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

public class WordCountTools extends Configured implements Tool {

    public static class TokenizerMapper
        extends Mapper<Object, Text, Text, IntWritable>{

        private final static IntWritable one = new IntWritable(1);
        private Text word = new Text();

        public void map(Object key, Text value, Context context
                        ) throws IOException, InterruptedException {
            StringTokenizer itr = new StringTokenizer(value.toString());
            while (itr.hasMoreTokens()) {
                word.set(itr.nextToken());
                context.write(word, one);
            }
        }
    }

    public static class IntSumReducer
        extends Reducer<Text,IntWritable,Text,IntWritable> {

        private IntWritable result = new IntWritable();

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
                           ) throws IOException, InterruptedException {
            int sum = 0;
            for (IntWritable val : values) {
                sum += val.get();
            }
            result.set(sum);
            context.write(key, result);
        }
    }


    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new WordCountTools(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {

        if (args.length<2){
            System.out.println("WordCountTools <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out);
            System.out.println("");
            return -1;
        }

        Configuration conf = this.getConf();

        // Create job
        Job job = Job.getInstance(conf, "word count");
        job.setJarByClass(WordCountTools.class);

        job.setMapperClass(TokenizerMapper.class);
        job.setCombinerClass(IntSumReducer.class);
        job.setReducerClass(IntSumReducer.class);

        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        FileInputFormat.addInputPath(job, new Path(args[0]));
            //job.setInputFormatClass(TextInputFormat.class);

        FileOutputFormat.setOutputPath(job, new Path(args[1]));
            //job.setOutputFormatClass(TextOutputFormat.class);

        // Execute job and return status
        return job.waitForCompletion(true) ? 0 : 1;
    }
}
```

#### Modify Code

```bash
[student1@hadoopedge wordcount]$ nano WordCountTools.java
[student1@hadoopedge wordcount]$ cat WordCountTools.java
```

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

/**
 * WordCountTools demonstrates using the Hadoop Tool interface.
 * It counts the frequency of words in a given input text file(s).
 */
public class WordCountTools extends Configured implements Tool {

    // Mapper class: processes input key-value pairs and produces intermediate key-value pairs.
    public static class TokenizerMapper extends Mapper<Object, Text, Text, IntWritable> {

        private final static IntWritable one = new IntWritable(1); // Fixed value to emit for each word.
        private Text word = new Text(); // Text object to store each word.

        /**
         * The map method tokenizes each line of the input and emits key-value pairs
         * where the key is a word and the value is '1'.
         */
        public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
            StringTokenizer itr = new StringTokenizer(value.toString());
            while (itr.hasMoreTokens()) {
                word.set(itr.nextToken()); // Set the next word.
                context.write(word, one); // Emit the word with a count of 1.
            }
        }
    }

    // Reducer class: aggregates intermediate key-value pairs to produce final output.
    public static class IntSumReducer extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable result = new IntWritable(); // Stores the final count for a word.

        /**
         * The reduce method sums up all values (word counts) for a given key (word).
         */
        public void reduce(Text key, Iterable<IntWritable> values, Context context)
                throws IOException, InterruptedException {
            int sum = 0; // Initialize the sum for each word.
            for (IntWritable val : values) {
                sum += val.get(); // Add up counts for the current word.
            }
            result.set(sum); // Set the total count for the word.
            context.write(key, result); // Emit the word and its total count.
        }
    }

    /**
     * The main method invokes the ToolRunner to run the program with the given arguments.
     */
    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new WordCountTools(), args);
        System.exit(res); // Exit with the status returned by ToolRunner.
    }

    /**
     * The run method contains the main logic for the WordCount job.
     * It sets up the configuration, job parameters, and executes the MapReduce job.
     */
    @Override
    public int run(String[] args) throws Exception {

        // Print all arguments provided to the program.
        System.out.println("Arguments provided:");
        for (String arg : args) {
            System.out.println(arg); // Print each argument.
        }

        // Validate the arguments: at least two (input and output paths) are required.
        if (args.length < 2) {
            System.out.println("Usage: WordCountTools <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out); // Print help for generic options.
            return -1; // Exit with error code if arguments are insufficient.
        }

        // Retrieve the configuration object.
        Configuration conf = this.getConf();

        // Create and configure a new Hadoop job.
        Job job = Job.getInstance(conf, "word count"); // Set the job name.
        job.setJarByClass(WordCountTools.class); // Set the job's main class.

        // Set Mapper and Reducer classes.
        job.setMapperClass(TokenizerMapper.class);
        job.setCombinerClass(IntSumReducer.class); // Combiner to optimize intermediate results.
        job.setReducerClass(IntSumReducer.class);

        // Set output key and value types.
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        // Specify input and output paths from arguments.
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        // Execute the job and return the status.
        return job.waitForCompletion(true) ? 0 : 1; // Return 0 if successful, else 1.
    }
}
```

##### Key Changes Made

Print Command-Line Arguments: Added a loop in the run method to print all arguments passed to the program.

```java
System.out.println("Arguments provided:");
for (String arg : args) {
    System.out.println(arg);
}
```

#### Clear output, Compile the Code, Create the JAR

```bash
[student1@hadoopedge wordcount]$ hdfs dfs -rm -skipTrash -r wordcount/output
Deleted wordcount/output
[student1@hadoopedge wordcount]$ javac -version
javac 21.0.5
[student1@hadoopedge wordcount]$ java -version
openjdk version "21.0.5" 2024-10-15 LTS
OpenJDK Runtime Environment (Red_Hat-21.0.5.0.10-1) (build 21.0.5+10-LTS)
OpenJDK 64-Bit Server VM (Red_Hat-21.0.5.0.10-1) (build 21.0.5+10-LTS, mixed mode, sharing)
[student1@hadoopedge wordcount]$ javac -classpath $(hadoop classpath) -d . -source 1.8 -target 1.8 WordCountTools.java
warning: [options] bootstrap class path not set in conjunction with -source 8
warning: [options] source value 8 is obsolete and will be removed in a future release
warning: [options] target value 8 is obsolete and will be removed in a future release
warning: [options] To suppress warnings about obsolete options, use -Xlint:-options.
4 warnings
[student1@hadoopedge wordcount]$ jar -cvf WordCountTools.jar -C . .
added manifest
adding: WordCount$IntSumReducer.class(in = 1739) (out= 739)(deflated 57%)
adding: WordCount$TokenizerMapper.class(in = 1736) (out= 754)(deflated 56%)
adding: WordCount.class(in = 1491) (out= 814)(deflated 45%)
adding: WordCount.java(in = 2089) (out= 702)(deflated 66%)
adding: WordCountTools$IntSumReducer.class(in = 1789) (out= 766)(deflated 57%)
adding: WordCountTools$TokenizerMapper.class(in = 1786) (out= 786)(deflated 55%)
adding: WordCountTools.class(in = 2218) (out= 1157)(deflated 47%)
adding: WordCountTools.java(in = 4783) (out= 1627)(deflated 65%)
adding: input/(in = 0) (out= 0)(stored 0%)
adding: input/file01.txt(in = 26) (out= 23)(deflated 11%)
adding: input/file02.txt(in = 28) (out= 24)(deflated 14%)
adding: wc.jar(in = 3051) (out= 2701)(deflated 11%)
```

#### Run the Program, and Check the Output

```bash
[student1@hadoopedge wordcount]$ hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/WordCountTools.jar WordCount wordcount/input wordcount/output
2024-12-16 18:21:25,317 INFO client.DefaultNoHARMFailoverProxyProvider: Connecting to ResourceManager at hadoopname.dat351/10.0.0.175:8032
2024-12-16 18:21:25,708 WARN mapreduce.JobResourceUploader: Hadoop command-line option parsing not performed. Implement the Tool interface and execute your application with ToolRunner to remedy this.
2024-12-16 18:21:25,727 INFO mapreduce.JobResourceUploader: Disabling Erasure Coding for path: /tmp/hadoop-yarn/staging/student1/.staging/job_1734003739512_0009
2024-12-16 18:21:25,997 INFO input.FileInputFormat: Total input files to process : 2
2024-12-16 18:21:26,091 INFO mapreduce.JobSubmitter: number of splits:2
2024-12-16 18:21:26,272 INFO mapreduce.JobSubmitter: Submitting tokens for job: job_1734003739512_0009
2024-12-16 18:21:26,272 INFO mapreduce.JobSubmitter: Executing with tokens: []
2024-12-16 18:21:26,436 INFO conf.Configuration: found resource resource-types.xml at file:/usr/local/hadoop/3.3.4/etc/hadoop/resource-types.xml
2024-12-16 18:21:26,501 INFO impl.YarnClientImpl: Submitted application application_1734003739512_0009
2024-12-16 18:21:26,553 INFO mapreduce.Job: The url to track the job: http://hadoopname.dat351:8088/proxy/application_1734003739512_0009/
2024-12-16 18:21:26,554 INFO mapreduce.Job: Running job: job_1734003739512_0009
2024-12-16 18:21:44,071 INFO mapred.ClientServiceDelegate: Application state is completed. FinalApplicationStatus=SUCCEEDED. Redirecting to job history server
2024-12-16 18:21:44,604 INFO mapreduce.Job: Job job_1734003739512_0009 running in uber mode : false
2024-12-16 18:21:44,605 INFO mapreduce.Job:  map 100% reduce 100%
2024-12-16 18:21:44,653 INFO mapreduce.Job: Job job_1734003739512_0009 completed successfully
...
...
...
[student1@hadoopedge wordcount]$ hdfs dfs -ls wordcount/output
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 18:21 wordcount/output/_SUCCESS
-rw-r--r--   3 student1 student1         35 2024-12-16 18:21 wordcount/output/part-r-00000
[student1@hadoopedge wordcount]$ hdfs dfs -cat "wordcount/output/*"
Goodbye 2
Hadoop  2
Hello   2
World   2
```

## Task 3

### Writing Your Own Hadoop Programs

#### Copy the files from `/share/dat351/java` into currect dir and look at the files.

```bash
[student1@hadoopedge wordcount]$ cp /share/dat351/java/* .
[student1@hadoopedge wordcount]$ ls -l
total 24
-rw-r--r-- 1 student1 student 3760 Dec 16  2024 CombinerCount.java
drwxr-xr-x 2 student1 student   40 Dec 16 16:14 input
-rw-r--r-- 1 student1 student 3062 Dec 16  2024 MapperCount.java
-rw-r--r-- 1 student1 student 3115 Dec 16  2024 ReducerCount.java
-rw-r--r-- 1 student1 student 3661 Dec 16  2024 ReducerCountWithCombiner.java
drwxr-xr-x 2 student1 student  136 Dec 16 18:50 WordCount
drwxr-xr-x 2 student1 student 4096 Dec 16 18:49 WordCountTools
```

`[student1@hadoopedge wordcount]$ cat CombinerCount.java`

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.TaskAttemptID;
import org.apache.hadoop.mapreduce.TaskID;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

public class CombinerCount extends Configured implements Tool {

    public static class CombinerCountMapper
        extends Mapper<Object, Text, Text, IntWritable>{

        private final static Text outkey = new Text("minvalue");
        private final static IntWritable one = new IntWritable(1);

        public void map(Object key, Text value, Context context
            ) throws IOException, InterruptedException {
            context.write(outkey, one);
        }
    }

    public static class CombinerCountCombiner
        extends Reducer<Text,IntWritable,Text,IntWritable> {

        private Text combiner;
        private IntWritable listlength = new IntWritable();

        public void setup(Context context) throws IOException, InterruptedException {
            TaskAttemptID taskattempt = context.getTaskAttemptID();
            TaskID taskID = taskattempt.getTaskID();
            int combinerID = taskID.getId();
            combiner = new Text("Combiner-" + combinerID);
        }

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
            ) throws IOException, InterruptedException {

            int count = 0;
            for (IntWritable val : values) {
                ++count;
            }
            listlength.set(count);
            context.write(combiner, listlength);
        }
    }

    public static class CombinerCountReducer
        extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable result = new IntWritable();

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
            ) throws IOException, InterruptedException {

            int combinecount = 0;
            for (IntWritable val : values) {
                // List will have length one
                combinecount += val.get();
            }
            result.set(combinecount);
            context.write(key, result);
        }
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new CombinerCount(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {

        if (args.length<2){
            System.out.println("CombinerCount <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out);
            System.out.println("");
            return -1;
        }

        Configuration conf = this.getConf();
        Job job = Job.getInstance(conf, "Combiner count");
        job.setJarByClass(CombinerCount.class);
        job.setMapperClass(CombinerCountMapper.class);
        job.setCombinerClass(CombinerCountCombiner.class);
        job.setReducerClass(CombinerCountReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        return job.waitForCompletion(true) ? 0 : 1;
    }
}
```

`[student1@hadoopedge wordcount]$ cat MapperCount.java`

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.TaskAttemptID;
import org.apache.hadoop.mapreduce.TaskID;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

public class MapperCount extends Configured implements Tool {

    public static class MapperCountMapper
        extends Mapper<Object, Text, Text, IntWritable>{

        private final static IntWritable one = new IntWritable(1);
        private Text mapper;

        public void setup(Context context) throws IOException, InterruptedException {
            TaskAttemptID taskattempt = context.getTaskAttemptID();
            TaskID taskID = taskattempt.getTaskID();
            int mapperID = taskID.getId();
            mapper = new Text("Mapper-" + mapperID);
        }

        public void map(Object key, Text value, Context context
            ) throws IOException, InterruptedException {
            context.write(mapper, one);
        }
    }

    public static class MapperCountReducer
        extends Reducer<Text, IntWritable, Text, IntWritable> {

        private IntWritable result = new IntWritable();

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
            ) throws IOException, InterruptedException {

            int mapcount = 0;
            for (IntWritable val : values) {
                // List will have length one
                mapcount += val.get();
            }
            result.set(mapcount);
            context.write(key, result);
        }
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new MapperCount(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {

        if (args.length<2){
            System.out.println("MapperCount <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out);
            System.out.println("");
            return -1;
        }

        Configuration conf = this.getConf();
        Job job = Job.getInstance(conf, "Mapper count");
        job.setJarByClass(MapperCount.class);
        job.setMapperClass(MapperCountMapper.class);
        job.setReducerClass(MapperCountReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        return job.waitForCompletion(true) ? 0 : 1;
    }
}
```

`[student1@hadoopedge wordcount]$ cat ReducerCount.java`

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.TaskAttemptID;
import org.apache.hadoop.mapreduce.TaskID;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

public class ReducerCount extends Configured implements Tool {

    public static class ReducerCountMapper
        extends Mapper<Object, Text, Text, IntWritable>{

        private final static Text outkey = new Text("thekey");
        private final static IntWritable one = new IntWritable(1);

        public void map(Object key, Text value, Context context
                        ) throws IOException, InterruptedException {
            context.write(outkey, one);
        }
    }

    public static class ReducerCountReducer
        extends Reducer<Text, IntWritable, Text, IntWritable> {

        private Text reducer;
        private IntWritable listlength = new IntWritable();

        public void setup(Context context) throws IOException, InterruptedException {
            TaskAttemptID taskattempt = context.getTaskAttemptID();
            TaskID taskID = taskattempt.getTaskID();
            int reducerID = taskID.getId();
            reducer = new Text("Reducer-" + reducerID);
        }

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
                           ) throws IOException, InterruptedException {

            int count = 0;
            for (IntWritable val : values) {
                ++count;
            }
            listlength.set(count);
            context.write(reducer, listlength);
        }
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new ReducerCount(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {

        if (args.length<2){
            System.out.println("ReducerCount <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out);
            System.out.println("");
            return -1;
        }

        Configuration conf = this.getConf();
        Job job = Job.getInstance(conf, "Reducer count");
        job.setJarByClass(ReducerCount.class);
        job.setMapperClass(ReducerCountMapper.class);
        job.setReducerClass(ReducerCountReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        return job.waitForCompletion(true) ? 0 : 1;
    }
}
```

`[student1@hadoopedge wordcount]$ cat ReducerCountWithCombiner.java`

```java
import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.TaskAttemptID;
import org.apache.hadoop.mapreduce.TaskID;

import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.conf.Configured;

public class ReducerCountWithCombiner extends Configured implements Tool {

    public static class ReducerCountMapper
        extends Mapper<Object, Text, Text, IntWritable>{

        private final static Text outkey = new Text("minvalue");
        private final static IntWritable one = new IntWritable(1);

        public void map(Object key, Text value, Context context
            ) throws IOException, InterruptedException {
            context.write(outkey, one);
        }
    }

    public static class ReducerCountCombiner
        extends Reducer<Text,IntWritable,Text,IntWritable> {

        private final static Text outkey = new Text("minvalue");
        private final static IntWritable one = new IntWritable(1);

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
            ) throws IOException, InterruptedException {

            context.write(outkey, one);
        }
    }

    public static class ReducerCountReducer
        extends Reducer<Text, IntWritable, Text, IntWritable> {

        private Text reducer;
        private IntWritable listlength = new IntWritable();

        public void setup(Context context) throws IOException, InterruptedException {
            TaskAttemptID taskattempt = context.getTaskAttemptID();
            TaskID taskID = taskattempt.getTaskID();
            int reducerID = taskID.getId();
            reducer = new Text("Reducer-" + reducerID);
        }

        public void reduce(Text key, Iterable<IntWritable> values,
                           Context context
            ) throws IOException, InterruptedException {

            int count = 0;
            for (IntWritable val : values) {
                ++count;
            }
            listlength.set(count);
            context.write(reducer, listlength);
        }
    }

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new ReducerCountWithCombiner(), args);
        System.exit(res);
    }

    @Override
    public int run(String[] args) throws Exception {

        if (args.length<2){
            System.out.println("ReducerCountWithCombiner <inDir> <outDir>");
            ToolRunner.printGenericCommandUsage(System.out);
            System.out.println("");
            return -1;
        }

        Configuration conf = this.getConf();
        Job job = Job.getInstance(conf, "Reducer count with combiner");
        job.setJarByClass(ReducerCountWithCombiner.class);
        job.setMapperClass(ReducerCountMapper.class);
        job.setCombinerClass(ReducerCountCombiner.class);
        job.setReducerClass(ReducerCountReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        return job.waitForCompletion(true) ? 0 : 1;
    }
}
```

#### Create Output Directories in HDFS for the respective classes

```bash
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_count_numbers
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_find_smallest
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_find_largest
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_sum_numbers
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_digit_sum
```

##### Organize the Previous Output Folders, Remove Old Output

```bash
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mkdir -p /user/student1/wordcount/output_wordcount_tools
[student1@hadoopedge myHadoopPrograms]$ hdfs dfs -mv /user/student1/wordcount/output /user/student1/wordcount/output_wordcount
```

```bash
# Remove the previous wordcount output directory
hdfs dfs -rm -r /user/student1/wordcount/output_wordcount
hdfs dfs -rm -r /user/student1/wordcount/output_wordcount_tools

# Remove the previous output directories for each of the tasks
hdfs dfs -rm -r /user/student1/wordcount/output_count_numbers
hdfs dfs -rm -r /user/student1/wordcount/output_find_smallest
hdfs dfs -rm -r /user/student1/wordcount/output_find_largest
hdfs dfs -rm -r /user/student1/wordcount/output_sum_numbers
hdfs dfs -rm -r /user/student1/wordcount/output_digit_sum
```

#### Create Input Directories in HDFS, Re-run Jobs, and Check Output

```bash
# Create Input folders by copying from local
[student1@hadoopedge input]$ hadoop fs -copyFromLocal /share/home/student1/dat351/hadoop/hadoop-files/wordcount/input/* /user/student1/wordcount/input/
[student1@hadoopedge input]$ hadoop fs -ls /user/student1/wordcount/input
Found 2 items
-rw-r--r--   3 student1 student1         26 2024-12-16 21:06 /user/student1/wordcount/input/file01.txt
-rw-r--r--   3 student1 student1         28 2024-12-16 21:06 /user/student1/wordcount/input/file02.txt
[student1@hadoopedge input]$ hadoop fs -cat /user/student1/wordcount/input/file01.txt
Hello World Goodbye World
[student1@hadoopedge input]$ hadoop fs -cat /user/student1/wordcount/input/file02.txt
Hello Hadoop Goodbye Hadoop

# Run the WordCount job with the input and output directories
hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/WordCount/WordCount.jar WordCount /user/student1/wordcount/input /user/student1/wordcount/output_wordcount

# Run the WordCountTools job with the input and output directories
hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/WordCountTools/WordCountTools.jar WordCountTools /user/student1/wordcount/input /user/student1/wordcount/output_wordcount_tools

# Check output
[student1@hadoopedge input]$ hadoop fs -ls /user/student1/wordcount/output_wordcount
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 21:10 /user/student1/wordcount/output_wordcount/_SUCCESS
-rw-r--r--   3 student1 student1         35 2024-12-16 21:10 /user/student1/wordcount/output_wordcount/part-r-00000
[student1@hadoopedge input]$ hadoop fs -cat /user/student1/wordcount/output_wordcount/part-r-00000
...
...
...
2024-12-16 21:12:14,719 INFO hdfs.DFSClient: Successfully connected to /10.0.0.178:9866 for BP-651351515-10.0.0.175-1695113042748:blk_1073744480_3657
Goodbye 2
Hadoop  2
Hello   2
World   2
[student1@hadoopedge input]$ hadoop fs -ls /user/student1/wordcount/output_wordcount_tools
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 21:10 /user/student1/wordcount/output_wordcount_tools/_SUCCESS
-rw-r--r--   3 student1 student1         35 2024-12-16 21:10 /user/student1/wordcount/output_wordcount_tools/part-r-00000
[student1@hadoopedge input]$ hadoop fs -cat /user/student1/wordcount/output_wordcount_tools/part-r-00000
Goodbye 2
Hadoop  2
Hello   2
World   2
```

#### Create Input Directories in HDFS for mapReduceNrw

```bash
# Create Input folders by copying from local
[student1@hadoopedge input]$ hadoop fs -copyFromLocal /share/dat351/input/* /user/student1/mapReduceNrs/input/
[student1@hadoopedge input]$ hadoop fs -ls /user/student1/mapReduceNrs/input/
```

#### Create a Directory and Javas File for Counting Numbers

```bash
[student1@hadoopedge CountNumbers]$ pwd
/share/home/student1/dat351/hadoop/hadoop-files/wordcount/CountNumbers
[student1@hadoopedge CountNumbers]$ ls -l
total 28
-rw-r--r-- 1 student1 student 1215 Dec 16 20:04 CountNumbers.java
-rw-r--r-- 1 student1 student  536 Dec 16 20:05 CountNumbersMapper.java
-rw-r--r-- 1 student1 student  623 Dec 16 20:06 CountNumbersReducer.java
```

`[student1@hadoopedge CountNumbers]$ cat CountNumbers.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.fs.Path;
import java.io.IOException;

public class CountNumbers {

    public static void main(String[] args) throws Exception {
        // Set up the job
        Job job = Job.getInstance();
        job.setJarByClass(CountNumbers.class);
        job.setJobName("Count Numbers");

        // Set the input and output directories
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        // Set the Mapper and Reducer classes
        job.setMapperClass(CountNumbersMapper.class);
        job.setReducerClass(CountNumbersReducer.class);

        // Set the output key and value types
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        // Wait for the job to complete
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
```

`[student1@hadoopedge CountNumbers]$ cat CountNumbersMapper.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import java.io.IOException;

public class CountNumbersMapper extends Mapper<Object, Text, Text, IntWritable> {
    private final static Text outkey = new Text("totalNumbers");
    private final static IntWritable one = new IntWritable(1);

    public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        context.write(outkey, one); // Increment the total count
    }
}
```

`[student1@hadoopedge CountNumbers]$ cat CountNumbersReducer.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import java.io.IOException;

public class CountNumbersReducer extends Reducer<Text, IntWritable, Text, IntWritable> {
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        int sum = 0;
        for (IntWritable val : values) {
            sum += val.get();
        }
        result.set(sum);
        context.write(key, result); // Output the total count
    }
}
```

#### Compile the Java Files With Java 8 and Create the JAR

```bash
javac -classpath $(hadoop classpath) -source 1.8 -target 1.8 *.java
warning: [options] bootstrap class path not set in conjunction with -source 8
warning: [options] source value 8 is obsolete and will be removed in a future release
warning: [options] target value 8 is obsolete and will be removed in a future release
warning: [options] To suppress warnings about obsolete options, use -Xlint:-options.
4 warnings
[student1@hadoopedge CountNumbers]$ ls -l
total 28
-rw-r--r-- 1 student1 student 1283 Dec 16 20:12 CountNumbers.class
-rw-r--r-- 1 student1 student 2819 Dec 16 20:06 CountNumbers.jar
-rw-r--r-- 1 student1 student 1215 Dec 16 20:04 CountNumbers.java
-rw-r--r-- 1 student1 student 1523 Dec 16 20:12 CountNumbersMapper.class
-rw-r--r-- 1 student1 student  536 Dec 16 20:05 CountNumbersMapper.java
-rw-r--r-- 1 student1 student 1741 Dec 16 20:12 CountNumbersReducer.class
-rw-r--r-- 1 student1 student  623 Dec 16 20:06 CountNumbersReducer.java
[student1@hadoopedge CountNumbers]$ jar -cvf CountNumbers.jar *.class
added manifest
adding: CountNumbers.class(in = 1283) (out= 728)(deflated 43%)
adding: CountNumbersMapper.class(in = 1523) (out= 621)(deflated 59%)
adding: CountNumbersReducer.class(in = 1741) (out= 740)(deflated 57%)
```

#### Clean Output Folder and Run Hadoop Job

```bash
[student1@hadoopedge CountNumbers]$ hadoop fs -rm -r /user/student1/wordcount/output_count_numbers
2024-12-16 20:21:16,587 INFO fs.TrashPolicyDefault: Moved: 'hdfs://hadoopname.dat351:9000/user/student1/wordcount/output_count_numbers' to trash at: hdfs://hadoopname.dat351:9000/user/student1/.Trash/Current/user/student1/wordcount/output_count_numbers
```

#### Create a Directory and Javas File for Finding Smallest Number

```bash
[student1@hadoopedge FindSmallest]$ ls -l
total 12
-rw-r--r-- 1 student1 student 1215 Dec 16 20:30 FindSmallest.java
-rw-r--r-- 1 student1 student  659 Dec 16 20:34 FindSmallestMapper.java
-rw-r--r-- 1 student1 student  807 Dec 16  2024 FindSmallestReducer.java
```

`[student1@hadoopedge FindSmallest]$ cat FindSmallest.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.fs.Path;
import java.io.IOException;

public class FindSmallest {

    public static void main(String[] args) throws Exception {
        // Set up the job
        Job job = Job.getInstance();
        job.setJarByClass(FindSmallest.class);
        job.setJobName("Find Smallest");

        // Set the input and output directories
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        // Set the Mapper and Reducer classes
        job.setMapperClass(FindSmallestMapper.class);
        job.setReducerClass(FindSmallestReducer.class);

        // Set the output key and value types
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(IntWritable.class);

        // Wait for the job to complete
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
```

`[student1@hadoopedge FindSmallest]$ cat FindSmallestMapper.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import java.io.IOException;

public class FindSmallestMapper extends Mapper<Object, Text, Text, IntWritable> {

    private final static Text outkey = new Text("smallest");

    public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
        try {
            int number = Integer.parseInt(value.toString());
            context.write(outkey, new IntWritable(number));
        } catch (NumberFormatException e) {
            // Handle the case where the value is not an integer
        }
    }
}
```

`[student1@hadoopedge FindSmallest]$ cat FindSmallestReducer.java`

```java
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import java.io.IOException;

public class FindSmallestReducer extends Reducer<Text, IntWritable, Text, IntWritable> {
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        int smallest = Integer.MAX_VALUE;

        // Iterate over the values and find the smallest one
        for (IntWritable val : values) {
            int number = val.get();
            if (number < smallest) {
                smallest = number;
            }
        }

        result.set(smallest);
        context.write(key, result); // Output the smallest value
    }
}
```

#### Compile the Java Files With Java 8 and Create the JAR

```bash
[student1@hadoopedge FindSmallest]$ javac -classpath $(hadoop classpath) -source 1.8 -target 1.8 *.java
warning: [options] bootstrap class path not set in conjunction with -source 8
warning: [options] source value 8 is obsolete and will be removed in a future release
warning: [options] target value 8 is obsolete and will be removed in a future release
warning: [options] To suppress warnings about obsolete options, use -Xlint:-options.
4 warnings
[student1@hadoopedge FindSmallest]$ jar -cvf FindSmallest.jar *.class
added manifest
adding: FindSmallest.class(in = 1283) (out= 730)(deflated 43%)
adding: FindSmallestMapper.class(in = 1661) (out= 706)(deflated 57%)
adding: FindSmallestReducer.class(in = 1787) (out= 772)(deflated 56%)
[student1@hadoopedge FindSmallest]$ ls -l
total 28
-rw-r--r-- 1 student1 student 1283 Dec 16 20:38 FindSmallest.class
-rw-r--r-- 1 student1 student 2936 Dec 16 20:39 FindSmallest.jar
-rw-r--r-- 1 student1 student 1215 Dec 16 20:30 FindSmallest.java
-rw-r--r-- 1 student1 student 1661 Dec 16 20:38 FindSmallestMapper.class
-rw-r--r-- 1 student1 student  659 Dec 16 20:34 FindSmallestMapper.java
-rw-r--r-- 1 student1 student 1787 Dec 16 20:38 FindSmallestReducer.class
-rw-r--r-- 1 student1 student  807 Dec 16 20:35 FindSmallestReducer.java
```

#### Run Count Numbers and Find Smallest in Hadoop and Check Output

```bash
# Run the CountNumbers job with the input and output directories
hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/CountNumbers/CountNumbers.jar CountNumbers /user/student1/mapReduceNrs/input/ /user/student1/mapReduceNrs/output_count_nrs

# Run the FindSmallest job with the input and output directories
hadoop jar /share/home/student1/dat351/hadoop/hadoop-files/wordcount/FindSmallest/FindSmallest.jar FindSmallest /user/student1/mapReduceNrs/input/ /user/student1/mapReduceNrs/output_find_smallest

# Check output
[student1@hadoopedge FindSmallest]$ hadoop fs -ls /user/student1/mapReduceNrs/output_count_nrs
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 22:04 /user/student1/mapReduceNrs/output_count_nrs/_SUCCESS
-rw-r--r--   3 student1 student1         21 2024-12-16 22:04 /user/student1/mapReduceNrs/output_count_nrs/part-r-00000
[student1@hadoopedge FindSmallest]$ hadoop fs -cat /user/student1/mapReduceNrs/output_count_nrs/part-r-00000
...
...
...
2024-12-16 22:06:32,661 INFO hdfs.DFSClient: Successfully connected to /10.0.0.179:9866 for BP-651351515-10.0.0.175-1695113042748:blk_1073744505_3682
totalNumbers    3000000
[student1@hadoopedge FindSmallest]$ hadoop fs -ls /user/student1/mapReduceNrs/output_find_smallest
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 22:04 /user/student1/mapReduceNrs/output_find_smallest/_SUCCESS
-rw-r--r--   3 student1 student1         15 2024-12-16 22:04 /user/student1/mapReduceNrs/output_find_smallest/part-r-00000
[student1@hadoopedge FindSmallest]$ hadoop fs -cat /user/student1/mapReduceNrs/output_find_smallest/part-r-00000
...
...
...
2024-12-16 22:08:03,319 INFO hdfs.DFSClient: Successfully connected to /10.0.0.178:9866 for BP-651351515-10.0.0.175-1695113042748:blk_1073744515_3692
smallest        -9999
```
