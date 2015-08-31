#include "header.h"

int queue_attachment();
int send_registration(int id_queue, resource rsc_wished[], int num_rsc_wished);

int main(int argc, char const *argv[])
{
  /*
  Data Structures:
    -> resource 
      -> rsc_wished      : Wished resource
    -> integer  
      -> num_rsc_wished  : Resource wished_resource dimension
      -> IPC_Key         : IPC key 
      -> id_queue       : key queue 
      -> i               : loop index   
  */
  resource rsc_wished[MAX_RSC_GAMBLER];
  int num_rsc_wished, id_queue, i;

  int pid_process;

  printf("\t\t*****************\n\t\t*   Gambler     *\n\t\t***************"
          "**\n\n\n\n");
  printf("My ID is°: %d\n", getpid() );

  load_resource(argv[1],rsc_wished, &num_rsc_wished);
  printf("I want the following resource:\n ");
  for(i=0; i < num_rsc_wished; i++)
    printf("-> %d %s for %d € each\n", rsc_wished[i].quantity, rsc_wished[i].name, rsc_wished[i].price);

  id_queue = queue_attachment();
  send_registration(id_queue, rsc_wished, num_rsc_wished);

  printf("Waiting for TAO...\n");
  msg_alert m2;
  for (i = 0; i < num_rsc_wished; i++) {
    int rd_bytes = msgrcv(id_queue, &m2, sizeof(msg_alert) - sizeof(long), getpid(), 0);
    if (rd_bytes>0){
      if (m2.key_shm){
        pid_process = fork();
        if (pid_process == -1) {
          perror("Error while generating tao agent... ");
          exit(EXIT_FAILURE);
        }
        else if(!pid_process){
          char id_shma[10], id_smpa[10];
          sprintf(id_shma,"%d",m2.key_shm);
          sprintf(id_smpa,"%d",m2.key_smp);
          char* arg[] = {
                          "agent.out",
                          m2.resource.name,
                          id_shma,
                          id_smpa,
                          NULL
                        };
          execve(arg[0], arg, NULL);
          perror("Error on execve... ");
          exit(EXIT_FAILURE);
        }
        printf("Agent lunched for %s\n", m2.resource.name);
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



int queue_attachment(){
  int IPC_Key;
  IPC_Key = ipc_key_creation();

  printf("Opening messagge queue... ");
  int id_queue = msgget(IPC_Key, 0);
  if(id_queue == -1)
  {
    perror("FAIL. Error on msgget. ");
    exit(EXIT_FAILURE);
  }  
  printf("Done\n");
  return id_queue;
}


int send_registration(int id_queue,resource rsc_wished[], int num_rsc_wished){
  int i;
  rsc_msg msg_request;

  
  msg_request.type = RES_REQ_TYPE;
  msg_request.pid = getpid();
  msg_request.num_resource = num_rsc_wished;
  for (i = 0; i < num_rsc_wished; i++)
    msg_request.data[i] = rsc_wished[i];

  //invia messaggio con le risorse
  printf("Send registration message... ");
  while(msgsnd(id_queue, &msg_request, sizeof(rsc_msg) - sizeof(long), 0)==-1)
  {
    perror("Trying to send registration message... \n");
    sleep(1);
  }
  printf("Done!\n");

  return 0;
}