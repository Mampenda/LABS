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
2024-12-16 17:20:10,522 INFO mapreduce.Job: Counters: 54
        File System Counters
                FILE: Number of bytes read=83
                FILE: Number of bytes written=827568
                FILE: Number of read operations=0
                FILE: Number of large read operations=0
                FILE: Number of write operations=0
                HDFS: Number of bytes read=324
                HDFS: Number of bytes written=35
                HDFS: Number of read operations=11
                HDFS: Number of large read operations=0
                HDFS: Number of write operations=2
                HDFS: Number of bytes read erasure-coded=0
        Job Counters
                Launched map tasks=2
                Launched reduce tasks=1
                Data-local map tasks=2
                Total time spent by all maps in occupied slots (ms)=12194
                Total time spent by all reduces in occupied slots (ms)=9748
                Total time spent by all map tasks (ms)=6097
                Total time spent by all reduce tasks (ms)=2437
                Total vcore-milliseconds taken by all map tasks=6097
                Total vcore-milliseconds taken by all reduce tasks=2437
                Total megabyte-milliseconds taken by all map tasks=12486656
                Total megabyte-milliseconds taken by all reduce tasks=9981952
        Map-Reduce Framework
                Map input records=2
                Map output records=8
                Map output bytes=86
                Map output materialized bytes=89
                Input split bytes=270
                Combine input records=8
                Combine output records=6
                Reduce input groups=4
                Reduce shuffle bytes=89
                Reduce input records=6
                Reduce output records=4
                Spilled Records=12
                Shuffled Maps =2
                Failed Shuffles=0
                Merged Map outputs=2
                GC time elapsed (ms)=156
                CPU time spent (ms)=1630
                Physical memory (bytes) snapshot=843587584
                Virtual memory (bytes) snapshot=11761209344
                Total committed heap usage (bytes)=705167360
                Peak Map Physical memory (bytes)=312102912
                Peak Map Virtual memory (bytes)=3361959936
                Peak Reduce Physical memory (bytes)=221212672
                Peak Reduce Virtual memory (bytes)=5038788608
        Shuffle Errors
                BAD_ID=0
                CONNECTION=0
                IO_ERROR=0
                WRONG_LENGTH=0
                WRONG_MAP=0
                WRONG_REDUCE=0
        File Input Format Counters
                Bytes Read=54
        File Output Format Counters
                Bytes Written=35
```

#### Check the Output

```bash
[student1@hadoopedge wordcount]$ hdfs dfs -ls wordcount/output
Found 2 items
-rw-r--r--   3 student1 student1          0 2024-12-16 17:20 wordcount/output/_SUCCESS
-rw-r--r--   3 student1 student1         35 2024-12-16 17:20 wordcount/output/part-r-00000
[student1@hadoopedge wordcount]$ hdfs dfs -cat "wordcount/output/*"
2024-12-16 17:26:43,347 WARN impl.BlockReaderFactory: I/O error constructing remote block reader.
java.net.ConnectException: Connection refused
        at sun.nio.ch.SocketChannelImpl.checkConnect(Native Method)
        at sun.nio.ch.SocketChannelImpl.finishConnect(SocketChannelImpl.java:716)
        at org.apache.hadoop.net.SocketIOWithTimeout.connect(SocketIOWithTimeout.java:205)
        at org.apache.hadoop.net.NetUtils.connect(NetUtils.java:586)
        at org.apache.hadoop.hdfs.DFSClient.newConnectedPeer(DFSClient.java:3033)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.nextTcpPeer(BlockReaderFactory.java:829)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.getRemoteBlockReaderFromTcp(BlockReaderFactory.java:754)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.build(BlockReaderFactory.java:381)
        at org.apache.hadoop.hdfs.DFSInputStream.getBlockReader(DFSInputStream.java:755)
        at org.apache.hadoop.hdfs.DFSInputStream.blockSeekTo(DFSInputStream.java:685)
        at org.apache.hadoop.hdfs.DFSInputStream.readWithStrategy(DFSInputStream.java:884)
        at org.apache.hadoop.hdfs.DFSInputStream.read(DFSInputStream.java:957)
        at java.io.DataInputStream.read(DataInputStream.java:100)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:94)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:68)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:129)
        at org.apache.hadoop.fs.shell.Display$Cat.printToStdout(Display.java:101)
        at org.apache.hadoop.fs.shell.Display$Cat.processPath(Display.java:96)
        at org.apache.hadoop.fs.shell.Command.processPathInternal(Command.java:370)
        at org.apache.hadoop.fs.shell.Command.processPaths(Command.java:333)
        at org.apache.hadoop.fs.shell.Command.processPathArgument(Command.java:306)
        at org.apache.hadoop.fs.shell.Command.processArgument(Command.java:288)
        at org.apache.hadoop.fs.shell.Command.processArguments(Command.java:272)
        at org.apache.hadoop.fs.shell.FsCommand.processRawArguments(FsCommand.java:121)
        at org.apache.hadoop.fs.shell.Command.run(Command.java:179)
        at org.apache.hadoop.fs.FsShell.run(FsShell.java:327)
        at org.apache.hadoop.util.ToolRunner.run(ToolRunner.java:81)
        at org.apache.hadoop.util.ToolRunner.run(ToolRunner.java:95)
        at org.apache.hadoop.fs.FsShell.main(FsShell.java:390)
2024-12-16 17:26:43,351 WARN hdfs.DFSClient: Failed to connect to /10.0.0.177:9866 for file /user/student1/wordcount/output/part-r-00000 for block BP-651351515-10.0.0.175-1695113042748:blk_1073744406_3583, add to deadNodes and continue.
java.net.ConnectException: Connection refused
        at sun.nio.ch.SocketChannelImpl.checkConnect(Native Method)
        at sun.nio.ch.SocketChannelImpl.finishConnect(SocketChannelImpl.java:716)
        at org.apache.hadoop.net.SocketIOWithTimeout.connect(SocketIOWithTimeout.java:205)
        at org.apache.hadoop.net.NetUtils.connect(NetUtils.java:586)
        at org.apache.hadoop.hdfs.DFSClient.newConnectedPeer(DFSClient.java:3033)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.nextTcpPeer(BlockReaderFactory.java:829)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.getRemoteBlockReaderFromTcp(BlockReaderFactory.java:754)
        at org.apache.hadoop.hdfs.client.impl.BlockReaderFactory.build(BlockReaderFactory.java:381)
        at org.apache.hadoop.hdfs.DFSInputStream.getBlockReader(DFSInputStream.java:755)
        at org.apache.hadoop.hdfs.DFSInputStream.blockSeekTo(DFSInputStream.java:685)
        at org.apache.hadoop.hdfs.DFSInputStream.readWithStrategy(DFSInputStream.java:884)
        at org.apache.hadoop.hdfs.DFSInputStream.read(DFSInputStream.java:957)
        at java.io.DataInputStream.read(DataInputStream.java:100)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:94)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:68)
        at org.apache.hadoop.io.IOUtils.copyBytes(IOUtils.java:129)
        at org.apache.hadoop.fs.shell.Display$Cat.printToStdout(Display.java:101)
        at org.apache.hadoop.fs.shell.Display$Cat.processPath(Display.java:96)
        at org.apache.hadoop.fs.shell.Command.processPathInternal(Command.java:370)
        at org.apache.hadoop.fs.shell.Command.processPaths(Command.java:333)
        at org.apache.hadoop.fs.shell.Command.processPathArgument(Command.java:306)
        at org.apache.hadoop.fs.shell.Command.processArgument(Command.java:288)
        at org.apache.hadoop.fs.shell.Command.processArguments(Command.java:272)
        at org.apache.hadoop.fs.shell.FsCommand.processRawArguments(FsCommand.java:121)
        at org.apache.hadoop.fs.shell.Command.run(Command.java:179)
        at org.apache.hadoop.fs.FsShell.run(FsShell.java:327)
        at org.apache.hadoop.util.ToolRunner.run(ToolRunner.java:81)
        at org.apache.hadoop.util.ToolRunner.run(ToolRunner.java:95)
        at org.apache.hadoop.fs.FsShell.main(FsShell.java:390)
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
2024-12-16 18:21:32,598 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 0 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:33,599 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 1 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:34,599 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 2 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:35,705 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 0 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:36,706 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 1 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:37,707 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 2 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:38,813 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 0 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:39,813 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 1 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:40,814 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 2 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:41,922 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 0 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:42,922 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 1 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:43,923 INFO ipc.Client: Retrying connect to server: hadoopa.dat351/10.0.0.177:41813. Already tried 2 time(s); retry policy is RetryUpToMaximumCountWithFixedSleep(maxRetries=3, sleepTime=1000 MILLISECONDS)
2024-12-16 18:21:44,071 INFO mapred.ClientServiceDelegate: Application state is completed. FinalApplicationStatus=SUCCEEDED. Redirecting to job history server
2024-12-16 18:21:44,604 INFO mapreduce.Job: Job job_1734003739512_0009 running in uber mode : false
2024-12-16 18:21:44,605 INFO mapreduce.Job:  map 100% reduce 100%
2024-12-16 18:21:44,653 INFO mapreduce.Job: Job job_1734003739512_0009 completed successfully
2024-12-16 18:21:44,740 INFO mapreduce.Job: Counters: 54
        File System Counters
                FILE: Number of bytes read=83
                FILE: Number of bytes written=827568
                FILE: Number of read operations=0
                FILE: Number of large read operations=0
                FILE: Number of write operations=0
                HDFS: Number of bytes read=324
                HDFS: Number of bytes written=35
                HDFS: Number of read operations=11
                HDFS: Number of large read operations=0
                HDFS: Number of write operations=2
                HDFS: Number of bytes read erasure-coded=0
        Job Counters
                Launched map tasks=2
                Launched reduce tasks=1
                Data-local map tasks=2
                Total time spent by all maps in occupied slots (ms)=18630
                Total time spent by all reduces in occupied slots (ms)=9716
                Total time spent by all map tasks (ms)=9315
                Total time spent by all reduce tasks (ms)=2429
                Total vcore-milliseconds taken by all map tasks=9315
                Total vcore-milliseconds taken by all reduce tasks=2429
                Total megabyte-milliseconds taken by all map tasks=19077120
                Total megabyte-milliseconds taken by all reduce tasks=9949184
        Map-Reduce Framework
                Map input records=2
                Map output records=8
                Map output bytes=86
                Map output materialized bytes=89
                Input split bytes=270
                Combine input records=8
                Combine output records=6
                Reduce input groups=4
                Reduce shuffle bytes=89
                Reduce input records=6
                Reduce output records=4
                Spilled Records=12
                Shuffled Maps =2
                Failed Shuffles=0
                Merged Map outputs=2
                GC time elapsed (ms)=307
                CPU time spent (ms)=1520
                Physical memory (bytes) snapshot=848871424
                Virtual memory (bytes) snapshot=11759628288
                Total committed heap usage (bytes)=762314752
                Peak Map Physical memory (bytes)=318951424
                Peak Map Virtual memory (bytes)=3360763904
                Peak Reduce Physical memory (bytes)=211165184
                Peak Reduce Virtual memory (bytes)=5038583808
        Shuffle Errors
                BAD_ID=0
                CONNECTION=0
                IO_ERROR=0
                WRONG_LENGTH=0
                WRONG_MAP=0
                WRONG_REDUCE=0
        File Input Format Counters
                Bytes Read=54
        File Output Format Counters
                Bytes Written=35
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
