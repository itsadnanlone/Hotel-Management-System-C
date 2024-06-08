#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<ctype.h>

#define MAXLEN 256
#define MAXITEMS 31
#define WaiterManagerSHM "waiter_manager.txt"
#define MangerAdminSHM "manager_admin.txt"
#define TableWaiterSHM "table_waiter.txt"


int main(){

	system("touch waiter_manager.txt");
	system("touch earnings.txt");

	int totalTables;

	printf("Enter the Total Number of Tables at the Hotel: ");
	scanf("%d",&totalTables);

	// need to communicate with all the waiters

	// initialised the shm, which may get updated by waiter
	for(int i=1;i<=totalTables;i++){
			// each table/waiter
			key_t key = ftok("waiter_manager.txt",i);
			if (key==-1){
				fprintf(stderr, "Key Generation Failed!\n");
				return 1;}
			int shmid = shmget(key,MAXLEN, IPC_CREAT| 0644);
			if(shmid ==-1){
				fprintf(stderr,"11SHM Creation Failed!");
				return 1;}
			int *shmPtr = shmat(shmid,NULL,0);
			if(shmPtr==(void*)-1){
				fprintf(stderr,"SHM Ptr Failed!");
				return 1;}
			shmPtr[0] = -1;
			shmdt(shmPtr);
	}

	// will escape through loop only when none of them is -1
	for(int i=1; i<=totalTables; i++){
		key_t key = ftok("waiter_manager.txt",i);
		int shmid = shmget(key,MAXLEN,0644);
		int *shmPtr = shmat(shmid,NULL,0);
		if(shmPtr[0] == -1){
			shmdt(shmPtr);
			i = 1;
			continue;
		}
	}

	// all waiters have send info
	int hotelEarnings=0;
	for(int i=1; i<=totalTables; i++){
		key_t key = ftok("waiter_manager.txt",i);
		int shmid = shmget(key,MAXLEN,0644);
		int* shmPtr = shmat(shmid,NULL,0);
		hotelEarnings+=shmPtr[0];
		FILE *file;
		file = fopen("earnings.txt","a");
		fprintf(file,"Earning from Table %d: %d INR\n",i,shmPtr[0]);
		fclose(file);
		shmdt(shmPtr);
	}
	

	// All tables have terminated!
	// Waiting for Termination Request from Admin

	key_t key = ftok(MangerAdminSHM,1);
	if(key == -1){
		fprintf(stderr, "Key Generation Failed!");
		return 1;}

	int shmid = shmget(key,sizeof(char),IPC_CREAT | 0766);

	if(shmid == -1){
		fprintf(stderr, "*SHM Generation Failed!");
		return 1;};

	char* shmPtr = shmat(shmid,NULL,0);

	while(1){
		if(shmPtr[0]=='Y'){
			break;
		}
	}

	printf("\n\n Admin has closed the hotel!");
	shmctl(shmid,IPC_RMID,0);


	int wagesOfWaiters = 0.4 * hotelEarnings;
	int profit = hotelEarnings - wagesOfWaiters;

	printf("\n\nTotal Earnings of Hotel: %d INR\n",hotelEarnings);
	printf("Total Wages of Waiters: %d INR\n",wagesOfWaiters);
	printf("Total Profit: %d INR\n",profit);

	FILE *file;
	file = fopen("earnings.txt","a");
	fprintf(file,"\n\nTotal Earnings of Hotel: %d INR\n",hotelEarnings);
	fprintf(file,"Total Wages of Waiters: %d INR\n",wagesOfWaiters);
	fprintf(file,"Total Profit: %d INR\n",profit);

	fclose(file);

	printf("\n\nManager is leaving...\n");
	printf("Thank you for visiting the Hotel!");

	//terminate all ipc constructs
	// table customers, pipes -> dont need
	// table waiter, sm -> below
	// waiter manager, sm -> below
	// hotel manager, admin, shmid -> done 

	for(int i=1;i<=totalTables;i++){
			// each waiter
			key_t key = ftok("waiter_manager.txt",i);

			if (key==-1){
				fprintf(stderr, "Key Generation Failed!\n");
				return 1;}

			int shmid = shmget(key,MAXLEN, IPC_CREAT| 0644);

			if(shmid ==-1){
				fprintf(stderr,"***SHM Creation Failed!");
				return 1;};

			shmctl(shmid,IPC_RMID,0);
	}

	// for(int i=1;i<=totalTables;i++){
	// 		// each table/waiter
	// 	printf("Hello jigara");
	// 		key_t key = ftok("table_waiter.txt",i);

	// 		if (key==-1){
	// 			fprintf(stderr, "Key Generation Failed!\n");
	// 			return 1;}

	// 		int shmid = shmget(key,MAXLEN, IPC_CREAT| 0644);

	// 		if(shmid ==-1){
	// 			fprintf(stderr,"****SHM Creation Failed!");
	// 			return 1;};

	// 		shmctl(shmid,IPC_RMID,0);
	// 		printf("Hello jigara");
	// }

	return 0;
}