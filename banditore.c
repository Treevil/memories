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


    // Inizializzazioni
    msg1 winners[num_req];
    int tao[MAX_CLIENT_R*MAX_CLIENT],semid[MAX_CLIENT_R*MAX_CLIENT];

    for(int i=0; i<num_req; i++)
    {
        winners[i].type=req[i].pid_applicant;
        winners[i].num_res=0;
    }

    for(int i=0;i<(MAX_CLIENT_R*MAX_CLIENT);i++)
    {
        semid[i]=0;
        tao[i]=0;
    }

    //ALERT: Inizio for della morte

    for(int i=0;i<num_resources;i++)
    {
        //Per ogni risorsa:
        // - creo un tavolo delle offerte (shared memory)
        // - creo un semaforo per far accedere in mutua esclusione
        // - mando un messaggio ai clienti interessati      

        tao[i] = new_sharedMemory(SHMKEY+(2*i));//crea ed inizializza tao
        semid[i] = new_semaphore(SEMKEY+(2*i));//crea semaforo
        printf("Aperto TAO per %s\n",band_resources[i].name);

        if(!alert_clients(qid,SHMKEY+(2*i),SEMKEY+(2*i),req,num_req,band_resources[i]))
        {
            printf("Errore avviso cliente\n");
        }


        int n[MAX_CLIENT_R*MAX_CLIENT];//pid figli


        //per ogni tao una fork
        //il figlio tiene il timer
        //if i+1 ecc ecc allora ciclo con MAX_TAO wait
        //alla fine wait sui tao rimasti

        n[i] = fork();
        if(n[i]==-1)
        {
            perror("Error while generating tao controller...");
            exit(EXIT_FAILURE);
        }
        else if(!n[i]){
            // Codice figlio
            /* ALERT: Cosa serve sto signal? */
            signal(SIGINT,SIG_IGN);

            // Countdown
            int timer=TAO_LAST;
            while(timer)
            {
                /* ALERT: Il banditore Dorme? */
                if(timer<4)
                    printf("Countdown TAO per %s: %ds\n",band_resources[i].name,timer);
                timer--;
                sleep(1);
            }
            //qui si stabiliscono i vincitori dell'asta
            //p su semaforo
            
            P(semid[i],0);
            //analisi tao
            /*attach*/
            offer *o = (offer *)shmat(tao[i], NULL, 0);
            if(o==-1)
            {
                perror("Error shared memory attach...");
                exit(EXIT_FAILURE);
            }

            //analizza_tao(num_ric,risorse[i],o,&vincite);
            if(i==TAO_S)
                print_tao(o);
            int m,cont=0;
            resource temp;
            FILE *f=fopen(band_resources[i].name,"w");
            do
            {
                m=max_of(o);
                cont++;
                if(!(o+m)->prize_each) break;

                band_resources[i].quantity-=(o+m)->quantity;

                for(int k=0;k<num_req;k++)
                {
                  if(winners[k].type==((long)(o+m)->pid_offer))
                  {
                      temp=band_resources[i];
                      temp.price=(o+m)->prize_each;
                      if(band_resources[i].quantity>0)
                        temp.quantity=(o+m)->quantity;
                      else
                      {
                        temp.quantity=(o+m)->quantity+band_resources[i].quantity;
                        band_resources[i].quantity=0;
                      }
                      winners[k].data[winners[k].num_res]=temp;
                      winners[k].num_res++;
                                
                      fprintf(f,"%li %d %d\n",winners[k].type,temp.quantity,temp.price);            
                  }
                }
                (o+m)->prize_each=0;
            }while(band_resources[i].quantity>0 && cont<MAX_TAO_OF);
            
            fclose(f);
            /*detach*/
            if(shmdt(o))
            {
                perror("Error shared memory detach...");
                exit(EXIT_FAILURE);
            }  
            if(tao[i]) 
            {
                deallocate_tao(tao[i]);
                tao[i]=0;
            }
            //v su semaforo
            V(semid[i],0);

            deallocate_semaphore(semid[i]);
            semid[i]=0;         

            printf("TAO per %s deallocato\n",band_resources[i].name);
            exit(EXIT_SUCCESS);
          }
          if((i+1)%MAX_TAO==0)
          {
            int k;
            int status = getpid(n[i]);
            for(k=0;k<MAX_TAO;k++)
            {
                wait(&status);
                tao[i+k]=0;
                semid[i+k]=0;
            }
          }
    }


    //mando i messaggi per le risorse non disponibili
    int trial=0;
    for(int i=0;i<num_req;i++)
    {
        int j;
        for(j=0;j<req[i].num_res;j++)
        {                                           
            if(!contains(band_resources,num_resources,req[i].resource[j].name))
            {
                msg2 m1;
                m1.type=req[i].pid_applicant;
                m1.shm_key=0;
                m1.smp_key=0;
                m1.res=req[i].resource[j];
                trial=0;
                while(msgsnd(qid,&m1,sizeof(msg2)-sizeof(long),0)==-1)
                {
                    if(trial==MAX_CLIENT)
                        exit(EXIT_FAILURE);
                    perror("Trying to advert client... \n");
                    sleep(0.1);
                    trial++;
                }
            }
        }
    }

    //leggi vincite
    for(int i=0;i<num_resources;i++)
    {
        int pid;
        FILE *f=fopen(band_resources[i].name,"r");
        while(fscanf(f,"%d",&pid)!=EOF)
        {
            int k;
            for(k=0;k<num_req;k++)
                if(winners[k].type==pid)
                {
                    fscanf(f,"%d",&(winners[k].data[winners[k].num_res].quantity));
                    fscanf(f,"%d",&(winners[k].data[winners[k].num_res].price));
                    strcpy(winners[k].data[winners[k].num_res].name,band_resources[i].name);
                    winners[k].num_res++;
                    fgetc(f);
                }
        }
        fclose(f);
    }

    //comunica eventuali vincite
    trial=0;
    for(int i=0;i<num_req;i++)
    {
        while(msgsnd(qid,&winners[i],sizeof(msg1)-sizeof(long),0)==-1)
        {
            if(trial==MAX_CLIENT)
                exit(EXIT_FAILURE);
            perror("Trying to advert client... \n");
            sleep(0.1);
            trial++;
        }
    }
    sleep(3);
    deallocate_queue(qid);
    qid=0;
    exit(EXIT_SUCCESS);
    return 0;

    return 0;
}


