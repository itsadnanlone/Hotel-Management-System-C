#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<string.h>
#include<ctype.h>
#include<unistd.h>

#define MangerAdminSHM "manager_admin.txt"
#define MAXLEN 256


int main(){

	system("touch manager_admin.txt");
	printf("Do you want to close the hotel?\n");
	printf("Enter Y for Yes: ");

	// initialise to N
	key_t key = ftok(MangerAdminSHM,1);
	if(key == -1){
		fprintf(stderr, "Key Generation Failed!");
		return 1;}

	int shmid = shmget(key,sizeof(char),IPC_CREAT | 0766);

	if(shmid == -1){
		fprintf(stderr, "SHM Generation Failed!");
		return 1;};

	char* shmPtr = shmat(shmid,NULL,0);

	shmPtr[0] = 'N';

	char closeHotel;

	while(1){
		scanf("%c",&closeHotel);

		if(closeHotel == 'Y' || closeHotel == 'y'){
			break;
		}
	}

	// inform the admin to terminate shm
	shmPtr[0] = 'Y';

	return 0;
}