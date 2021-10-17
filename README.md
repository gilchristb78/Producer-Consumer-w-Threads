# Producer-Consumer-w-Threads
This project implements a solution to the producer/consumer problem while implementing multiple threads with message passing. included is the .c file, a compiled file, and a typescript file showing an example output program.

This program creates a multitude of threads and impliments message passing between the various threads. there is a "main thread" that will send messages to each "child thread" that will add up all values it gets and keep track of certain statistics. the main thread will read from standard input and recieve 2 values, a value and a thread index. the main thread will then send a message to the thread with the given value and continue doing this until it reaches an EOF or a error on read. the children thread will then add up all values it has been sent, keeping track of the time it takes and wait for it to be told the program is finished. when an EOF is reached the main thread will tell the children process to finish, at which time they will all message the main thread with its count of how many messages it has recieved, value of the messages added up, and total time it has taken. which will be printed out by the main thread.

This program impliments multiple threads, producer / consumer problem, mutal exclusion, semaphors and more.

to run the program, compile the pcthreads.c file or use the pcthreads file. then run the program by calling ./pcthreads (#) to initiate the program with # threads. then simply type the messages by inputing 2 numbers seperated by a space ie. (==># #) with the value as the first number and the thread as the second value.  

Example:
./pcthreads 3
3 1
4 2
6 1
5 1
7 3
9 2
will output:
The result from thread 2 is 13 from 2 operations during 2 secs.
The result from thread 3 is 7 from 1 operations during 2 secs.
The result from thread 1 is 14 from 3 operations during 3 secs.

with varying times based on computer performance and varying order of output based on randomness.
