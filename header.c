#include "header.h"


int band_load_resource(const char* file_name, resource risorse[], int* n)
{
	//apertura file
	FILE *fp = fopen(file_name, "r");
	if(!fp) //se fallisce
	{
		perror("Error while opening the file.\n");
		return -1;
	}

	*n=0;
	//scorrimento e lettura file
	while(fscanf(fp,"%s",risorse[*n].name)!=EOF)
	{		
		fscanf(fp,"%d",&(risorse[*n].quantity));
		fscanf(fp,"%d",&(risorse[*n].price));
		fgetc(fp);
		(*n)++;
	}
	//chiusura file
	if(fclose(fp))
		return -1;//se fallisce chiusura
	return 1;
}


void printTAO_resources(resource arr[],int n)
{
	int i;
	for(i=0;i<n;i++)
		printf("-> %d %s a %d€\n",arr[i].quantity,arr[i].name,arr[i].price);
}


int client_load_resource(const char* file_name, resource risorse[], int*b, int* n)
{
	//apertura file
	FILE *fp = fopen(file_name, "r");
	if(!fp) //se fallisce
	{
		perror("Error while opening the file.\n");
		return -1;
	}

	//leggi budget in prima riga		
	fscanf(fp,"%d",b);
	//leggi il carattere di a capo
	fgetc(fp);
	
	*n=0;
	//scorrimento e lettura file
	while(fscanf(fp,"%s",risorse[*n].name)!=EOF)
	{		
		fscanf(fp,"%d",&(risorse[*n].quantity));
		fgetc(fp);
		risorse[*n].price=0;
		(*n)++;
	}
	//chiusura file
	if(fclose(fp))
		return -1;//se fallisce chiusura
	return 0;
}


void print_client_resource(resource arr[],int n)
{
	int i;
	for(i=0;i<n;i++)
		printf("-> %d %s\n",arr[i].quantity,arr[i].name);
}


int new_sharedMemory(int key)
{
	/*1- creazione*/
	int shmid = shmget(key, sizeof(offer)*MAX_TAO_OF, IPC_CREAT | 0600);
	if(shmid==-1)
	{
		perror("Error shared memory allocation...");
		exit(EXIT_FAILURE);
	}
	/*2- attach*/
	offer *o = (offer *)shmat(shmid, NULL, 0);
	if(o==-1)
	{
		perror("Error shared memory attach...");
		exit(EXIT_FAILURE);
	}
	/*3- inizializzazione*/
	int i;
	for(i=0;i<MAX_TAO_OF;i++)
	{
		(o+i)->pid_offer = -1;
		(o+i)->quantity = 0;
		(o+i)->prize_each = 0;
	}
	/*4- detach*/
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

	/* ALLERT arg.val?  */
	arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("Error while initializing semaphore...");
        exit(EXIT_FAILURE);
    }
	return semid;
}


int alert_clients(int queue,int shm,int sem,request req[],
						int num_req,resource res)
{
	int i,trial;
	for(i=0;i<num_req;i++)
	{
		if(contains(req[i].resource,req[i].num_res,res.name))
		{
			//invia messaggio con tipo=ric[i].pid_richiedente   (msgtype2)
			msg2 m1;
			m1.type=req[i].pid_applicant;
			m1.shm_key=shm;
			m1.smp_key=sem;
			m1.res=res;
			trial=0;
			/* ALERT: WHY While? */
			while(msgsnd(queue,&m1,sizeof(msg2)-sizeof(long),0)==-1)
			{
				if(trial==MAX_CLIENT)
					return -1;
				perror("Trying to advert client... \n");
				sleep(0.1);
				trial++;
			}
			printf("Cliente %ld avvertito per asta su %s\n",m1.type,res.name);
		}
	}
	return 0;
}


void deallocate_tao(int shmid)
{
	if (shmctl(shmid, IPC_RMID, NULL)==-1)
	{
		perror("Error while removing shared memory...");
		exit(EXIT_FAILURE);
	}
}


void deallocate_semaphore(int semid)
{
	if (semctl(semid, 0, IPC_RMID, NULL) == -1) {
        perror("Error while removing semaphore...");
        exit(EXIT_FAILURE);
    }
}


// Semaphore

int P(int semid, int semnum)
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

int V(int semid, int semnum)
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

int max_of(offer *o)
{
	int max=o->prize_each;
	int j=0;
	for(int i=1;i<MAX_TAO_OF;i++)
		if((o+i)->prize_each>max)
		{
			max=o->prize_each;
			j=i;
		}
	return j;
}


int contains(resource ris_ric[],int num_ris,char ris[])
{
	int i;
	for(i=0;i<num_ris;i++)
	{
		if(!strcmp(ris_ric[i].name,ris))
			return 0;
	}
	return 0;
}

void print_tao(offer *o)
{
	int i;
	for(i=0;i<MAX_TAO_OF;i++)
		printf("PID: %d, QNT: %d, €: %d;\n",(o+i)->pid_offer,(o+i)->quantity,(o+i)->prize_each);
}

void deallocate_queue(int qid)
{
	if (msgctl(qid, IPC_RMID, NULL)==-1)
	{
		perror("Error while removing queue...");
		exit(EXIT_FAILURE);
	}
}