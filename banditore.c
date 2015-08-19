/* 

File: Banditore
@Authors: Trevor Devalle, Marco Davi
 
 */

#include "header.h"


int main(int argc, char const *argv[])
{
	printf("Banditore Started...\n\n\n");
	//max n resource each client 
	resource band_resources[MAX_CLIENT_R*MAX_CLIENT];
	int num_resources = 0;

	// reading resource
	if(band_load_resource("resource.txt", band_resources, &num_resources) < 0){
		perror("Error while opening the file \n");
		exit(EXIT_FAILURE);
	}

	// print resource
	printf("Resources loaded:\n");
	printTAO_resources(band_resources,num_resources);

	// Creation unique key
	int key;
	if((key=ftok("banditore.c", 'G')) == -1)
	{
        perror("Error ftok...");
        exit(EXIT_FAILURE);
    }

	//Creation message queue
    int qid = msgget(key, 0666|IPC_CREAT);
    if(qid==-1)
	{
        perror("Creation messagge queue... FAIL!");
        exit(EXIT_FAILURE);
    }

	printf("Premere un tasto per leggere le richieste ");
	getchar();
	printf("Banditore in attesa di richieste...\n");



	msg1 m1;
	request req[MAX_CLIENT];
	int num_req = 0;

	int stop= -1 * TIME_FOR_REQUEST;
	while(stop)
	{
		int rd_bytes = msgrcv(qid, &m1, sizeof(msg1)-sizeof(long), RES_REQ_TYPE, IPC_NOWAIT);
		if(rd_bytes>0)
		{
			//ho letto il messaggio
			req[num_req].pid_applicant=m1.pid;
			req[num_req].num_res=m1.num_res;
			for(int i=0;i<m1.num_res;i++)
			{
				req[num_req].resource[i]=m1.data[i];
			}
			num_req++;
			printf("Ricevuto elenco da %d\n",m1.pid);
		}//rd_bytes <=0 non ho letto nulla dalla coda
		else if(errno == ENOMSG)
		{
			//La coda esiste, ma non contiene messaggi
			//oppure non contiene messaggi di tipo RES_REQ_TYPE
			//printf("\nNo request messagge in the queue...\n");
			sleep(1);
			stop++;
		}
		else if(errno==EINVAL || errno==EIDRM)
		{
			//la coda non esiste
			perror("\nQueue removed unexpectedly...");
			exit(EXIT_FAILURE);
		}
		else
		{
		  	perror("Other errors...");
		  	exit(EXIT_FAILURE);
		}
	}
	
	printf("\nIscrizioni chiuse, avvio tavoli delle offerte:\n\n");

	return 0;
}


