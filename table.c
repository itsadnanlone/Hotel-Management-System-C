#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<ctype.h>
#include<sys/ipc.h>

#define MAXLEN 256
#define MAXITEMS 31 // item0 -> 0
#define MAXTABLES 10
#define R 0
#define W 1
#define TableWaiterSHM "table_waiter.txt"

/*
example to get 2 Veg burgers and 1 chicken burger
input: 1 2 
	   2 1 
	   -1
ouput: 100
*/
int parseCost(char line[]){
	char *tokens[30]; // storing tokens

	char *token = strtok(line, " "); // returns first token
	int i = 0;
	while(token!=NULL){
		tokens[i]=token; 
		token = strtok(NULL," "); // continue tokenizing
		i++;
	}
	int cost = atoi(tokens[i-2]);
	return cost;
}


int main(){

	int tableNum, numOfCustomers;
	char fname[]="menu.txt";
	int costArray[MAXITEMS] ;
	// costArray[0] -> 0 
	// item 1 corresponds to costArray[1] and so on...
	

	system("touch menu.txt");
	system("touch table_waiter.txt");

	printf("Enter Table Number: ");
	scanf("%d",&tableNum);

	if(tableNum<1 || tableNum>10){
		fprintf(stderr, "Incorrect Table Number!");
		return 1;}

	printf("Enter Number of Customers at Table %d: ", tableNum);
	scanf("%d",&numOfCustomers);

	if(numOfCustomers<1 || numOfCustomers>10){
		fprintf(stderr, "Incorrect Number Of Customers!");
		return 1;}

	// file descriptors
	int fd[numOfCustomers][2];

	// pipe creation
	// int fd[2]; pipe(fd);
	for(int i=0; i<numOfCustomers; i++){
		int err = pipe(fd[i]);
		if(err == -1){
			fprintf(stderr, "Pipe Creation Failed\n" );
		}
	}


	// reading menu
	FILE *file;
	file = fopen(fname,"r");
	char line[MAXLEN];
	int item = 1;
	while(fgets(line,sizeof(line),file)!=NULL){
		printf("%s\n",line);
		int cost = parseCost(line);
		costArray[item]=cost;
		item++;
		// printf("%d",cost);
	}
	fclose(file);

	int totalItems = item;


	//flushing stdout from above
	fflush(stdout);

	// child creation
	for(int i=0; i<numOfCustomers; i++){
		pid_t pid = fork(); 
		/* runs previous things them moment is called - block printing 
		using fflush(stdout) */

		if(pid == 0){
			// for each customer
			int orderQuantityArray[MAXITEMS];// initialised to all 0
			printf("\nCustomer %d, Please Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done: \n",i+1);
			while(1){
				int itemNum, itemQuantity;
				printf("Enter Item: ");
				scanf("%d",&itemNum);
				if(itemNum == -1){
					break;
				}
				printf("Enter itemQuantity: ");
				scanf("%d", &itemQuantity);
				orderQuantityArray[itemNum]=itemQuantity;
			}
			// now the child has its order quantity array {0,2,1,0,0,0,..}
			// communicated with the table

			//What I am sending
			printf("\n");
			printf("I am Customer %d.\nMy order is: \n",i+1);

			for(int k=0; k<MAXITEMS; k++){
				if(orderQuantityArray[k]!=0){
					printf("I ordered %d pieces of item %d.\n", orderQuantityArray[k],k);
				}
			}
			close(fd[i][R]);
			write(fd[i][W], orderQuantityArray,sizeof(orderQuantityArray));
			close(fd[i][W]);

			// each child wrote in its corresponding pipe  
			exit(0);
			// each customer process terminates

			// orderQuantityArray 0 2 1 0 0 0 0 0 0 0 0...
		}
		else if(pid < 0){
			fprintf(stderr,"Fork Failed!");
			return 1;
		}
		else{
			// so that dusra customer wont access upward part!
			wait(NULL);
		}
	}

	// parent process starts

	// It should wait for all the customers to give order.
	// Needs to wait for all its children to terminate.
	// One wait gets vaporized by one child termination.
	for(int i=0; i<numOfCustomers; i++){
		wait(NULL);
	}
	// all customers ordered!

	// why to read from all tables?
	// only talk to waiter of my table!

	// just need to get a total Order Quantity of My Table by reading pipes
	// and then telling the Waiter about my totaltableOrder

	// total order quantity for the table

	int totalTableOrderQuantityArray[MAXITEMS]; // all 0
	// number of pipes is number of customers

	for(int i=0; i<numOfCustomers; i++){
		// for each customer/pipe
		int oneCustomerOrderQuantityArray[MAXITEMS];// initialised to all 0
		close(fd[i][W]);
		read(fd[i][R],oneCustomerOrderQuantityArray,sizeof(oneCustomerOrderQuantityArray));
		close(fd[i][R]);
		//printf("\noneCustomerOrderQuantityArray has # elements : %lu\n", sizeof(oneCustomerOrderQuantityArray)/sizeof(oneCustomerOrderQuantityArray[0]));
		// returns 31 correct

		// lets check we are getting correct order info for each customer
		printf("\n\nHello Customer %d. I am your Table Number %d speaking! You ordered:\n",i+1, tableNum);
		for(int j=0; j<MAXITEMS; j++){
			if(oneCustomerOrderQuantityArray[j]!=0){
				printf("You ordered %d pieces of item %d.\n",oneCustomerOrderQuantityArray[j],j);
			}
		}
		for(int k=0;k<MAXITEMS;k++){
			totalTableOrderQuantityArray[k]+=oneCustomerOrderQuantityArray[k];			
		}
	}

	// // I checked total cost works fine 
	// int totalCost = 0;
	// for(int i=0; i<MAXITEMS;i++){
	// 	totalCost += costArray[i]*totalTableOrderQuantityArray[i];
	// }


	// Communication with the Waiter through a SHM


	// Table tells the waiter about total orders quantity per item of its table.
	key_t key = ftok(TableWaiterSHM,tableNum);

	int shmid = shmget(key,sizeof(totalTableOrderQuantityArray)+sizeof(totalTableOrderQuantityArray[0]),IPC_CREAT | 0644);

	if(shmid == -1){
		fprintf(stderr, "Shared Memory Failed To Create!");
		return 1;}

	// Sending total order info 
	int * shmPtr = shmat(shmid, NULL,0);
	for(int i = 0; i<MAXITEMS; i++){
		shmPtr[i] = totalTableOrderQuantityArray[i];
	}

	// A check to know if waiter is still processing the amount!
	/*
		Waiter Process will change shmPtr[MAXIT] to 1
		I have taken care of shm size for this to not overflow!
		Also then,
		shmPtr[MAXITEMS] will be changed to -1 by table to inform 
		waiter to terminate!
		shmPtr[0] is updated by waiter to tell the amount for the table
	*/
	shmPtr[MAXITEMS] = 0;

	// Now we will wait for waiter to process the amount!
	printf("Waiting for the Waiter to Process!\n");
	while(1){
		if(shmPtr[MAXITEMS]==1){
			break;
		}
	}

	printf("Waiter has Processed!\n");
	int amount = shmPtr[0];
	printf("\n\nThe total bill amount is %d INR\n",amount);

	// inform waiter to terminate!
	printf("\n\nTelling Waiter to Terminate!!!");
	shmPtr[MAXITEMS] = -1;

	// table also terminates
	shmdt(shmPtr);
	sleep(1);

	shmctl(shmid,IPC_RMID,0);//let admin do this

	printf("\n\nTable %d Terminated!",tableNum);
	return 0;

}
/*
	int costArray[100] = {-1}; initialises only first element to -1
	// child creation
	for(int i=0; i<numOfCustomers; i++){
		pid_t pid = fork();
		if(pid == 0){
			break;
		}
	}
	char *token = strtok(char[] line, " ")
	need to run in a loop to get all the tokens, with NULL instead of line further
	--
	fflush(stdout)-->

	int main(){
		// fflush(stdout); -> niche wala phirbhi print hoga if used like this 
		printf("Hello HI");
		// fflush(stdout);
		fork();
		printf("Bye\t");

	}
	input taking is automatically flushed
	without fflush(stdout) -> Hello HIBye Hello HIBye
	with fflush(stdout) -> HelloHIBye Bye

*/