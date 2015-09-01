// utility.c
#include "header.h"

void P(int semid, int semnum)
{
	struct sembuf cmd;
	cmd.sem_num = semnum;
	cmd.sem_op = -1;
	cmd.sem_flg = 0;
	if(semop(semid, &cmd, 1))
	{
		perror("Error P on a semaphore...");
	}
}

void V(int semid, int semnum)
{
	struct sembuf cmd;
	cmd.sem_num = semnum;
	cmd.sem_op = 1;
	cmd.sem_flg = 0;
	if(semop(semid, &cmd, 1))
	{
		perror("Error V on a semaphore...");
	}
}



/*
Get data from file_name file and fill the resource data 
structure.
*/
int load_resource(const char* file_name, resource data[], int* length, int flag)
{
	// Open the file
	int budget;
	printf("Loading Resource....  ");
	FILE *fp = fopen(file_name, "r");
	if(!fp) //if something goes wrong...
	{
		perror("Error while opening the file. Resource cannot be loaded.\n");
		return -1;
	}
	if (flag == 0){
		//leggi budget in prima riga		
		fscanf(fp,"%d", &budget);
	}
	// Load data
	*length=0;
	while(fscanf(fp,"%s",data[*length].name)!=EOF)
	{
		fscanf(fp,"%d",&(data[*length].price));
		if (flag == 1)		
			fscanf(fp,"%d",&(data[*length].quantity));
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

// Just run the procedure for get a IPC_Key
int ipc_key_creation(){
	int IPC_Key;
	printf("Creation IPC Key... ");
    if((IPC_Key = ftok("auctioneer.c", 'G')) == -1)
    {
        perror("FAIL. Error on ftok...");
        exit(EXIT_FAILURE);
    }
    printf("Done\n");
    return IPC_Key;
}


int new_sharedmemory(int key)
{
	/* 1 - creazione*/
	int shmid = shmget(key, sizeof(bet)*MAX_BET, IPC_CREAT | 0600);
	if(shmid == -1)
	{
		perror("Error shared memory allocation...");
		exit(EXIT_FAILURE);
	}
	/* 2 - attach*/
	bet *o = (bet *)shmat(shmid, NULL, 0);
	if( (int) o == -1)
	{
		perror("Error shared memory attach...");
		exit(EXIT_FAILURE);
	}
	/* 3 - inizializzazione*/
	int i;
	for(i=0; i < MAX_BET; i++)
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



void detach_sharedMemory(int id_shm)
{

  if (shmctl(id_shm, IPC_RMID, NULL) == -1)
  {
    perror("Error while removing shared memory...");
    exit(EXIT_FAILURE);
  }
}

void detach_semaphore(int id_sem)
{
  if (semctl(id_sem, 0, IPC_RMID, NULL) == -1) {
        perror("Error while removing semaphore...");
        exit(EXIT_FAILURE);
    } 
}