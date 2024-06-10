// From Project Key Learnings......

admin->hotelmanager->table->waiter, scenario is considered


table,customers,waiter,hotelmanager,admin

**admin is executed. asks if we want to close the hotel. Already an shm is being created with the hotel maanger which is initialised to N. Once Y is entered into user, it terminates.
**hotel manager is executed. asks how many tables, suppose 2 entered. it creates two shm with the 2 waiters. (num of waiters = num of tables). I am supposed to update the
earnings.txt about total earning, profit and wages etc. It waits for all the tables to send info. It does it by creating shms with all and initiliases them with -1 and then waits 
till all of the shmPtr[0] with each waiter is no longer -1. Then it write into the earnings.txt. Now waits for admin to say close. wait is done by checking the shmPtr[0] with the hotel
admin, stuck in a loop till initialised N by admin isnt changed to Y by admin. Once Y, destroy all ipc constructs and then display closing of the hotel.
Checking the shmPtr[0] with admin condition is only checked, when waiter submits all info. Which ensures waiters and tables termination. Ensuring this ptr is checked only
wwhen no customer is left.
**#each,table is executed. asks how many customers, creates corresponding child processes, communicated about order info through oridinary pipes.
Then gives the order info the waiter, though shm. Waits for waiter to process info , like calculating total bill. When waiter returns back, it displays
total bill and tells the waiter to terminate. It then destroys the shm with waiter and terminates.
**waiter calculates the bill (using sleep we ensure all order info has been collected and table has sent this info using shm), and then it calculate total 
bill and then sends info to the table and the hotel manager. Now waits for the table to tell it to terminate.


Waiting is done by someone in a way, the one that waits initialise a ptr to something and then puts while(1) loop until someone changes it as intended
as it comes out of the loop.



******************** Creating n children and n pipes ********************



int fd[n][2];
// Pipes
for(int i=0; i<n; i++){
	pipe(fd[i]);
}
fflush(stdout);
for(int i=0; i<n; i++){
	pid_t pid = fork();

	if(pid == 0 ){
		// child process...
		// do something and then exit
		// like writing into pipes?
		close(fd[i][R]);
		write(fd[i][W],arr,sizeof(arr));
		close(fd[i][W]);

		exit(0);
	}
	else if(pid > 0){
		// parent process

		// to make sure new child is not created and doesnt start doing anything before previous child isnt terminated.
		wait(NULL);
	}
}

// wait for all children to terminate, no need of this though there
for(int i=0; i<n; i++){
	wait(NULL);
}




******************** Reading Menu and Parsing Cost ********************

FILE *file;
file = fopen("file.txt","r");

char line[MAXLEN];
int costArray[MAXITEMS];
int i=1;
while(fgets(line,MAXLEN+1,file)!=NULL){
	printf("%s\n",line);
	costArray[i] = parseCost(line);
	i++;
}

fclose(file);

int parseCost(char line[]){
	char *tokens[30];
	char *token = strtok(line," "); returns first token
	int i=0;
	while(token!=NULL){
		tokens[i] = token;
		token = strtok(NULL," "); continue tokenizing
		i++;
	}
	return atoi(tokens[i-2]);
}


******************** Communicating Waiting *************************

//table process sets shmPtr[MAXITEMS] = -1
//now a while true loop
while(1){
	if(shmPtr[MAXITEMS]==0){
		break;
		// waiter when completes processing, would make shmPtr[MAXITEMS] to 0 and table will come out of this while loop
	}
}



************************* Waiting till All Ptrs arent not -1 ********************
*hotelmanager

system("touch wa.txt");

// initialising to -1
for(int i=1; i<=totalTables;i++){
	key_t key = ftok("wa.txt",i);
	int shmid = shmget(key,sizeof(int),IPC_CREAT | 0766);
	int * shmPtr= shmat(shmid,NULL,0);
	shmPtr[0]=-1;
	shmdt(shmPtr); dont need to detach but can do
}

// waiting till all the shmPtr[0] aren't not -1
for(int i=1; i<=totalTables;i++){
	key_t key = ftok("wa.txt",i);
	int shmid = shmget(key,sizeof(int),IPC_CREAT | 0766);
	int * shmPtr= shmat(shmid,NULL,0);
	if(shmPtr[0]== -1){
		i=1;
		shmdt(shmPtr);// dont need to detach but can do
		continue;
	}
}



































// Important libararies
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> fork, excelp, etc..
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/msg.h>
															// Important Syntaxes

// Process Creation Related
pid_t pid
getpid();
getppid();

* excelp("path",arg0,arg1,..., NULL);
* sleep(2)

* 
int status;
wait(&status); return pid of terminated process
also can check status of its termination using status
For n children we need n wait(NULL) commands... for the parent to wait for all children
else if only once is used, any child''s termination would resume parent process 

* 
exit(status); terminates the child, this status will be referenced by the wait(&status);

* 
fork(); -> return 0 for cp, positive for pp, negative for error
when pp executes fork, children process is created and that process will get a copy of the all variables of the parent (wont share but copy).\

Also note, if parent has printf() statements previously it will execute them as well. To ignore them, we cann use 
fflush(stdout). 

Wherever we have place fflush(stdout), all printf statement before than would not get printed by the child. If we place the fflush(stdout) before fork(),
then no statement of parent will get printed by the child. 

*



// Pipes Anonymous or Ordinary (Not Named Pipes)
#define R 0
#define W 1
int fd[2]

int pipe(fd); 
pid_t pid = fork();
Anonymous pipes are for communication between parent and child processes only. Before forking, create a pipe. Pipe is like a special file, after forking
the child also gets its copy of the pipe.

If pipe returns -1, pipe creation failed.


int array[n];
close(fd[R/W]);
// read
read(fd[R],array,sizeof(array));
// write
write(fd[W],array,sizeof(array)); 
close(fd[W/R]);
size in bytes...




// Shared Memory

key_t key = ftok("file.txt",'A','B','C'/1,2,3) -> not "A","B", cant be basically type casted into ascii values
return -1 for error

int arr[MAXITEMS];

// creation or getting shmid
int shmid = shmget(key,sizeof(arr)+sizeof(arr[0]),IPC_CREAT | 0766)
returns -1 for error

// attaching to shm
int* shmPtr = shmat(shmid, NULL,0) 
char* 
--type cast 
returns (void*)-1 for error

Lets OS decide where it is attached, we wont worry about that.

// detaching 
int err = shmdt(shmPtr)
returns -1 for error

// controlling methods
returns -1 for error

// deleting shared memory
shmctl(shmid,IPC_RMID,0);

// meta data about shm
shmctl(shmid,IPC_STAT/IPC_SET,&buf)

buf.shm_segsz
buf.shm_nattch


// Message Queues

#define MAX_SIZE_OF_CONTENT_IN_BYTES 300
#define MAXLEN 100

struct message{
	long int msgtype;

	// optional, message text.. yea
	// can be any data type
	char msg_text;
	char msg_text[MAXLEN];
	int msg_arr[MAXLEN];

};

key_t key = ftok("file.txt",'A','B','C'/1,2,3) -> not "A","B","C"
returns -1 for error

// creation or getting msgid
int msgid = msgget(key,IPC_CREAT | 0766);
returns -1 for error

char content[MAXLEN] = "Hello";

struct message structmessage;


structmessage.msgtype = 1;
structmessage.msg_text = content // error use strcpy
strcpy(structmessage.msg_text,content);

// sending message
msgsnd(msgid,(void*)&structmessage,MAX_SIZE_OF_CONTENT_IN_BYTES,0)

If no content, size can be 0. Used for only sending msgtype. (maybe checking if such msgtype exists/not)

Flags used here:
IPC_NOWAIT : on off
used for conditions, if
message queue is full as per system imposed limit
placing this message will exceed the limit.

returns -1 for error

// receiving message

ssize_t size = msgrcv(msgid,(void*)&structmessage,MAX_SIZE_OF_CONTENT_IN_BYTES_WE_WISH_TO_RECEIVE, msgtype, 0)
returns -1 for error
returns size else

MESSAGETYPE: IF specified 
positive -> then corresponding first 
0 -> first one on the queue
negative -> first one on the queue that is <=|negative|

Again, IPC_NOWAIT...


****By default, IPC_WAIT, so blocking happens if it doesnt exist!!!!!!!*******
****In message queue, once something is read it is removed from the queue.*******

// commands
// deleting
msgctl(msgid,IPC_RMID,0)
// other methods

struct msqid_ds buf;

msgctl(msgid,IPC_STAT/IPC_SET,&buf)

// Files


FILE *file;

file = fopen("file.txt","r/w/a");

// writing
fprintf(file,"Hello")
			 "Hello %d",name);
			 name);

// reading
// single line input from user
fgets(line,MAXLEN,stdin);

//single line from a file
char line[MAXLEN];
while(fgets(line,MAXLEN+1,file)!=NULL){
		printf("%s",s);
}	

// Integer in each line!
char line[256];
while(fgets(line,MAXLEN+1,NULL)){
	int number = atoi(line);
}

fclose(file);

+1 so that MAXLEN number of characters can be stores else MAXLEN-1 (one being the null character).

























