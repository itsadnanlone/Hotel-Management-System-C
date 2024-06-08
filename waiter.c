#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/shm.h>

#define MAXITEMS 31
#define MAXLEN 256
#define MAXTABLES 10
#define TableWaiterSHM "table_waiter.txt"
#define WaiterManagerSHM "waiter_manager.txt"


int parseCost(char line[]){


	char *tokens[30];
	char *token = strtok(line," ");
	int i = 0;
	while(token!=NULL){
		tokens[i] = token;
		token = strtok(NULL," ");
		i++;
	}

	int cost = atoi(tokens[i-2]);
	return cost;
}

int main(){

	sleep(3);

	system("touch table_waiter.txt");
	system("touch waiter_manager.txt");


	// parsing cost of each item in the menu
	FILE *file;
	file = fopen("menu.txt","r");
	char line[MAXLEN];
	int costArray[MAXITEMS]; // all initialised to 0
	int i = 1;
	while(fgets(line,sizeof(line),file)!=NULL){
		int parsedCost = parseCost(line);
		costArray[i] = parsedCost;
		i++;
	}
	fclose(file);




	int waiterID;

	printf("Enter Waiter ID: ");
	scanf("%d",&waiterID);

	if(waiterID<1 || waiterID>10){
		fprintf(stderr, "Incorrect Waiter ID!\n");
		return 1;}

	int totalOrderQuantityArray[MAXITEMS];

	key_t key = ftok(TableWaiterSHM,waiterID);

	int shmid = shmget(key,sizeof(totalOrderQuantityArray)+sizeof(totalOrderQuantityArray[0]), IPC_CREAT | 0644);

	int * shmPtr = shmat(shmid,NULL,0);


	for(int i=0;i<MAXITEMS;i++){
		totalOrderQuantityArray[i]=shmPtr[i];
	}


	int totalAmount = 0;
	for(int i=0; i<MAXITEMS; i++){
		totalAmount+= costArray[i] * totalOrderQuantityArray[i];
	}	

	printf("Bill Amount for Table %d: %d INR\n", waiterID,totalAmount);

	// communicate to Table Process by changing shmPtr[MAXITEMS] to 1, but before also 
	// set amount to shmPtr[0]
	printf("Waiter sending bill information to the Table...\n\n");
	shmPtr[0] = totalAmount;
	shmPtr[MAXITEMS] = 1;


	// also communicate the total bill amount to the hotel manager process

	key_t mkey = ftok(WaiterManagerSHM,waiterID);
	int mshmid = shmget(mkey,2*sizeof(int),IPC_CREAT | 0644);
	int* mshmPtr= shmat(mshmid,NULL,0);

	mshmPtr[0]=totalAmount;
	shmdt(mshmPtr);



	// waiting for the table to tell waiter to terminate by setting
	// shm[MAXITEMS] to -1

	printf("Waiter waiting to terminate...\n");
	while(1){
		if(shmPtr[MAXITEMS]==-1){
			break;
		}
	}
	// Terminating
	shmdt(shmPtr);

	printf("\n\nWaiter %d Terminated ",waiterID);
	return 0;
}