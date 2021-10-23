#include "header.h"



int main(int argc, char const *argv[])
{
	// Data structure 
    resource resource[MAX_CLIENT_R]; //max n risorse per ogni cliente
	int budget, num_res,i;
	int n[MAX_CLIENT_R];



	//Loading Resource from file
	if(client_load_resource(argv[1],resource,&budget,&num_res)<0)
	{
		perror("Error while the client is opening the file...");
		exit(EXIT_FAILURE);
	}
	
	//stampa risorse
	printf("I have %d € and I have to buy:\n",budget);
	print_client_resource(resource,num_res);

	int key;

	for(i=0;i<MAX_CLIENT_R;i++)
		n[i]=0;

	if((key=ftok("banditore.c", 'G')) == -1)
	{
        perror("Error ftok...");
        exit(EXIT_FAILURE);
    }

	//si aggancia alla coda	di messaggi
	int qid=msgget(key,0);
	if(qid==-1)
	{
		perror("Queue messagge error...");
		exit(EXIT_FAILURE);
	}

	msg1 m1;
	m1.type=RES_REQ_TYPE;
	m1.pid=getpid();
	m1.num_res=num_res;
	for(i=0;i<num_res;i++)
		m1.data[i]=resource[i];

	//invia messaggio con le risorse
	while(msgsnd(qid,&m1,sizeof(msg1)-sizeof(long),0)==-1)
	{
		perror("Trying to send message... \n");
		sleep(1);
	}
	printf("Messaggio risorse inviato al banditore\n");
	sleep(1);
	//aspettare risposte
	//generare agenti per le offerte appena si apre un tao
	int stop=-5, read_all=num_res;
	int agenti=0;
	msg2 m2;


	while(read_all>0)
	{
		//recieve dei dati per accedere a tao e semafori
		int rd_bytes = msgrcv(qid, &m2, sizeof(msg2)-sizeof(long), getpid(), 0);
		if (rd_bytes>0)
		{
			//ho letto il messaggio
			//lancio agente con id di sem e tao e max budget per quella risorsa
			//fork+execve
			if(m2.shm_key)
			{
				n[read_all+num_res] = fork();
				if(n[read_all+num_res]==-1)
				{
					perror("Error while generating tao agent...");
			  		exit(EXIT_FAILURE);			
				}
				else if(!n[read_all+num_res])
				{
					//codice figlio
					//all'agente arriva il prezzo minimo
					char prezzo_minimo[10],quantita[10],ris_budget[10];
					char taoid[10], semid[10];
					sprintf(prezzo_minimo,"%d",m2.res.price);
					int j;
					for(j=0;j<num_res;j++)
						if(!strcmp(resource[j].name,m2.res.name))
							break;
					sprintf(quantita,"%d",resource[j].quantity);
					sprintf(ris_budget,"%d",budget/num_res);
					sprintf(taoid,"%d",m2.shm_key);
					sprintf(semid,"%d",m2.smp_key);
					char* arg[] = {
									"agente",
									m2.res.name,
									taoid,
									semid,
									prezzo_minimo,
									quantita,
									ris_budget,
									NULL
								  };
					execve(arg[0],arg,NULL);
					perror("Execve error...");
					exit(EXIT_FAILURE);
				}
				printf("Generato agente per TAO di %s\n",m2.res.name);
				agenti++;
			}
			else
				printf("Per la risorsa %s non e' stato " 
						"generato alcun TAO\n",m2.res.name);
			read_all--;
		}//rd_bytes <=0 non ho letto nulla dalla coda
		else if(errno == EINVAL || errno == EIDRM)
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

	for(i=0;i<agenti;i++)
		wait(&agenti);
	
	msg1 v;
	int rd_bytes = msgrcv(qid, &v, sizeof(msg1)-sizeof(long), getpid(), 0);
	if (rd_bytes>0)
	{
		//ho letto il messaggio
		if(v.num_res)
		{
			printf("Mi sono aggiudicato:\n");
			for(i=0;i<v.num_res;i++)
			{
				printf("-> %d %s per un totale di %d€\n",
					v.data[i].quantity,v.data[i].name,v.data[i].price*v.data[i].quantity);
				budget-=(v.data[i].price*v.data[i].quantity);
			}
			printf("Mi sono rimasti %d€\n",budget);
		}
		else
			printf("Non mi sono aggiudicato nessuna risorsa :(\n");		
	}//rd_bytes <=0 non ho letto nulla dalla coda
	else if (errno == ENOMSG)
	{
		//La coda esiste, ma non contiene messaggi
		//oppure non contiene messaggi di tipo pid
		sleep(0.5);
		stop++;
	}
	else if (errno == EINVAL || errno == EIDRM)
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
	exit(EXIT_SUCCESS);
	return 0;

}



