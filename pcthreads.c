/* pcthreads.c */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#define MAXTHREADS 10
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
struct msg {
int iFrom;  /*who sent the message (0 .. number-of-threads)*/
int value;  /*its value*/
int cnt;    /*count of operations (not needed by all msgs)*/
int tot;    /*total time (not needed by all msgs)*/
};


/* gcc -o pcthreads pcthreads.c -lpthread */


sem_t csems[MAXTHREADS],psems[MAXTHREADS];
int n; //number of threads
struct msg mailBox[MAXTHREADS];
int main(int argc,char *argv[])
{
   if(argc < 2){
	n = MAXTHREADS;
    } else {			//set number of threads to be 1-10
	n = MAX(1,
		MIN(MAXTHREADS,
			atoi(argv[1])+1
		    )
		);
   }
	
	
    pthread_t idprod; /* ids of threads */
    pthread_t idcons[n];

    void *producer();
    void *adder(void *);
    
    for(int i = 0; i < n; i++){	 		//make all consumber semaphors
	if (sem_init(&csems[i], 0, 0) < 0) {
        	perror("sem_init");
        	exit(1);
    	}
    }
    for(int i = 0; i < n; i++){		//make all producer semaphors
	if (sem_init(&psems[i], 0, 1) < 0) {
        	perror("sem_init");
        	exit(1);
    	}
    }
    if (pthread_create(&idprod, NULL, producer, 0) != 0) { //make main thread
        perror("pthread_create");
        exit(1);
    }
    for(long i = 1; i < n; i++){		//make all child threads
	    if (pthread_create(&idcons[i], NULL, adder, (void *)i) != 0) {
		perror("pthread_create");
		exit(1);
	    }	
    }
	
    (void)pthread_join(idprod, NULL);	//wait for main thread to be done
	for(int i = 1; i < n; i++){
    (void)pthread_join(idcons[i], NULL);	//wait for children to be done
	}
    for(int i = 0; i < n; i++){
    	(void)sem_destroy(&psems[i]);	//clean up semaphors
    	(void)sem_destroy(&csems[i]);
    }
    
}        


void *producer()
{
	void RecvMsg(int iRecv, struct msg*pMsg);
	void SendMsg(int iTo, struct msg*pMsg);
	struct msg mes;
	mes.iFrom = 0;		//set up all variables to be used
	mes.value = 0;
	int loc, val;
	char str[500] = "";
	char *token;
	while(fgets(str,500,stdin) != NULL){

		if(strcmp(str,"\n") == 0) //if input is \n => EOF
			break;

		val = atoi(strtok(str," "));
		token = strtok(NULL," ");
		if(!token)
			break;		//if only 1 arg EOF
		loc = atoi(token);
		token = strtok(NULL," ");
		if(token)			//if >2 args EOF
			break;
		if(loc >= n || loc < 1){		//make sure thread# is 0-n	
			printf("Not a valid thread, specify a number between 1 and %d\n",n-1);
			continue;
		}
		if(val < 0){	//make sure val is positive
			printf("First value must be positive\n");
			continue;
		}
		mes.value = val;
		SendMsg(loc,&mes); //send val to thread
	}


	mes.value = -1;
	for(int i = 1; i < n; i++){ //send terminate method to all threads
		SendMsg(i,&mes);
	}


	for(int i = 1; i < n; i++){ //recieve all values from threads
		RecvMsg(0,&mes);
		printf("the result from thread %d is %d from %d operations in %d seconds\n",mes.iFrom,mes.value,mes.cnt,mes.tot);  //print all values from threads
	}
	
}

void *adder(void *arg)
{


	void SendMsg(int iTo, struct msg*pMsg);
	void RecvMsg(int iRecv, struct msg*pMsg); 

	struct msg mes;	//initalize variables
	int val = 0;	//value
	int c = 0;	//count

	time_t start = time(NULL);
	while(time){
		RecvMsg((long)arg,&mes);	//find a message
		if(mes.value < 0){
			break;		//if message is EOF (-1) stop
		}
		val += mes.value;
		c++;			
		sleep(3);
	}
	time_t end = time(NULL);
	long timer = end - start;	//calculate time

	mes.iFrom = (long)arg;
	mes.value = val;	//make a message with final totals
	mes.cnt = c;
	mes.tot = (int)timer;
	SendMsg(0,&mes); //send final totals to main thread(0)

	
}

void SendMsg(int iTo, struct msg*pMsg)
{
	sem_wait(&psems[iTo]); //wait if mailbox full

	struct msg mes;
	mes.iFrom = pMsg->iFrom;
	mes.value = pMsg->value;	//make a message to put in the mailbox with values of pMsg
	mes.cnt = pMsg->cnt;
	mes.tot = pMsg->tot;
        mailBox[iTo] = mes;//put in mailbox

        sem_post(&csems[iTo]);//mailbox can be read from
    
}       

void RecvMsg(int iRecv, struct msg*pMsg)
{
	sem_wait(&csems[iRecv]); //wait if mailbox empty

	pMsg->value = mailBox[iRecv].value;
	pMsg->iFrom = mailBox[iRecv].iFrom;
	pMsg->cnt = mailBox[iRecv].cnt;		//set all values of pMsg to what is in the mailbox
	pMsg->tot = mailBox[iRecv].tot;

        sem_post(&psems[iRecv]); //mailbox can be filled
    
}
