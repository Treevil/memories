#include "header.h"


int main(int argc, char const *argv[])
{
  /*
  Data Structures:
    -> resource 
      -> rsc_wished      : Wished resource
    -> inìt  
      -> num_rsc_whised  : Resource wished_resource dimension
      -> IPC_Key         : IPC key 
      -> key_queue       : key queue 
      -> i               : loop index   
  */
  resource rsc_wished[MAX_RSC_GAMBLER];
  int num_rsc_whised, IPC_Key, key_queue, i;

  printf("\t\t*****************\n\t\t*   Gambler     *\n\t\t***************"
          "**\n\n\n\n");
  printf("My ID is°: %d\n", getpid() );

  load_resource(argv[1],rsc_wished, &num_rsc_whised);
  printf("I want the following resource:\n ");
  for(i=0; i < num_rsc_whised; i++)
    printf("-> %d %s for %d€ each\n", rsc_wished[i].quantity, rsc_wished[i].name, rsc_wished[i].price);

  printf("Creation IPC Key... ");
  if((IPC_Key = ftok("gambler.c", 'G')) == -1)
  {
    perror("FAIL. Error on ftok... ");
    exit(EXIT_FAILURE);
  }
  printf("Okay\n");

  printf("Opening messagge queue... ");
  key_queue = msgget(IPC_Key,0);
  if(key_queue==-1)
  {
    perror("FAIL. Error on msgget. ");
    exit(EXIT_FAILURE);
  }  
  printf("Okay\n");

  rsc_msg msg_request;
  msg_request.type = RES_REQ_TYPE;
  msg_request.pid = getpid();
  msg_request.num_resource = num_rsc_whised;
  for (i = 0; i < num_rsc_whised; i++)
    msg_request.data[i] = rsc_wished[i];
  //invia messaggio con le risorse
  printf("Registration... ");
  while(msgsnd(key_queue,&msg_request,sizeof(rsc_msg)-sizeof(long),0)==-1)
  {
    perror("Trying to send message... \n");
    sleep(1);
  }
  printf("Done!\n");
  sleep(1);


  msg_alert m2;
  for (i = 0; i < num_rsc_whised; i++) {
    int rd_bytes = msgrcv(key_queue, &m2, sizeof(msg_alert)-sizeof(long), getpid(), 0);
    if (rd_bytes>0){
      if (m2.key_shm){
        printf("Sono stato avvertito per la risorsa %s\n", m2.resource.name);
      } 
      else {
        printf("Per la risorsa %s non e' stato generato alcun TAO\n", m2.resource.name);
      }
    }//rd_bytes <=0 non ho letto nulla dalla coda
    else if(errno == EINVAL || errno == EIDRM)
    {
      //la coda non esiste
      perror("\nQueue removed unexpectedly...");
      exit(EXIT_FAILURE);
    }
    else
    {
        perror("Unknown error...");
        exit(EXIT_FAILURE);
    }
  }

  
}