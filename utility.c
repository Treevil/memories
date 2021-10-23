// utility.c
#include "header.h"

/*
Used for load resouce from file. 
*/
int load_resource(const char* file_name, resource data[], int* length)
{
	// Open the file
	printf("Loading Resource....  ");
	FILE *fp = fopen(file_name, "r");
	if(!fp) //if something goes wrong...
	{
		printf("FAIL \n");
		perror("Error while opening the file. Resource cannot be loaded.\n");
		return -1;
	}

	// Load data
	*length=0;
	while(fscanf(fp,"%s",data[*length].name)!=EOF)
	{		
		fscanf(fp,"%d",&(data[*length].quantity));
		fscanf(fp,"%d",&(data[*length].price));
		fgetc(fp);
		(*length)++;
	}
	//Close the file
	if(fclose(fp)) {
		printf("WARNING\n");
		perror("Error while closing the file. Resource loaded anyway.\n");
		return -1; 
	}

	printf("Ok \n");
	return 0;
}

int new_sharedmemory(int key)
{
	/* 1 - creazione*/
	int shmid = shmget(key, sizeof(bet)*MAX_TAO_OF, IPC_CREAT | 0600);
	if(shmid == -1)
	{
		perror("Error shared memory allocation...");
		exit(EXIT_FAILURE);
	}
	/* 2 - attach*/
	bet *o = (bet *)shmat(shmid, NULL, 0);
	if(o == -1)
	{
		perror("Error shared memory attach...");
		exit(EXIT_FAILURE);
	}
	/* 3 - inizializzazione*/
	int i;
	for(i=0; i<MAX_TAO_OF; i++)
	{
		(o+i)->pid_gambler = -1;
		(o+i)->quantity = 0;
		(o+i)->price_each = 0;
	}
	/* 4 - detach*/
	if(shmdt(o))
	{
		perror("Error shared memory detach...");
		exit(EXIT_FAILURE);
	}
	return shmid;
}

int new_semaphore(int key)
{
	int semid;
    union semun arg;
	//alloca semaforo
	if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) {
        perror("Error while creating semaphore...");
        exit(EXIT_FAILURE);
    }
	//inizializza semaforo a 1
	arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("Error while initializing semaphore...");
        exit(EXIT_FAILURE);
    }
	return semid;
}