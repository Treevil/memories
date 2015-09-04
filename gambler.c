/*
Specifications:
The process customer reads files from resources that must acquire and their quantity.
The file also shows the budget available to the customer.
At startup, the process registers itself to Auctioneer by sending a message containing 
its pid and a list of resources is concerned. The customer waits for the Auctioneer will 
inform it of the opening of a TAO for a resource of interest.  Upon receiving this message, 
the client initiates a process agent who will actually make the offer.
Started the Agent, the customer back waiting for a new message from the Auctioneer.
If the message sent by the Auctioneer inform it of the acquisition of an asset, 
the gambler will write a log file after him acquired the resource, the amount
and the total cost.

@Author: Devalle Trevor
*/



#include "header.h"

int pid_process[MAX_RSC_GAMBLER];

int queue_attachment();
int send_registration(int id_queue, resource rsc_wished[], int num_rsc_wished);
void rcvmsg_victory(int id_queue);


void handler_int(int s)
{
  int i;
  for(i=0;i<MAX_RSC_GAMBLER;i++)
  {
    if(pid_process[i])
      kill(pid_process[i],SIGKILL);
  }
  exit(EXIT_SUCCESS);
}


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

  int pid_process[MAX_RSC_GAMBLER];
  int gambler_budget = 20000;

  signal(SIGINT,handler_int);

  printf("\t\t*****************\n\t\t*   Gambler     *\n\t\t***************"
          "**\n\n\n\n");
  printf("My ID is°: %d\n", getpid() );
  // TODO: Set flag == 0
  load_resource(argv[1],rsc_wished, &num_rsc_wished, 1);
  printf("I want the following resource:\n ");
  for(i=0; i < num_rsc_wished; i++)
    printf("-> %d %s for %d € each\n", rsc_wished[i].quantity, rsc_wished[i].name, rsc_wished[i].price);

  id_queue = queue_attachment();
  send_registration(id_queue, rsc_wished, num_rsc_wished);

  printf("Waiting for TAO...\n");
  msg_alert m2;
  for (i = 0; i < num_rsc_wished; i++) {
    long rd_bytes = msgrcv(id_queue, &m2, sizeof(msg_alert) - sizeof(long), getpid(), 0);
    if (rd_bytes>0){
      if (m2.key_shm){
        pid_process[i] = fork();
        if (pid_process[i] == -1) {
          perror("Error while generating tao agent... ");
          exit(EXIT_FAILURE);
        }
        else if(!pid_process[i]){
          char id_shma[10], id_smpa[10], rsc_min_price[10],
            quantity[10], budget[10], id_queuea[10];
          sprintf(id_shma,"%d", m2.key_shm);
          sprintf(id_smpa,"%d", m2.key_smp);
          sprintf(rsc_min_price,"%d", m2.resource.price);
          sprintf(quantity,"%d", rsc_wished[i].quantity);
          sprintf(budget,"%d", gambler_budget / num_rsc_wished);
          sprintf(id_queuea,"%d", id_queue);
          char* arg[] = {
                          "agent.out",
                          m2.resource.name,
                          id_shma,
                          id_smpa,
                          rsc_min_price,
                          quantity,
                          budget,
                          id_queuea,
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

  rcvmsg_victory(id_queue);

  printf("Game Over.\n");
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

void rcvmsg_victory(int id_queue) {
  rsc_msg recived_msg;
  int i;
  long rcv_bytes = msgrcv(id_queue, &recived_msg, sizeof(rsc_msg) - sizeof(long), getpid(), 0);
  if(rcv_bytes > 0)
  {
    printf("I got: \n");
    for (i = 0; i < recived_msg.num_resource; i++)
    printf("%s\t", recived_msg.data[i].name);
    printf("%d\t", recived_msg.data[i].quantity);
    printf("%d\n", recived_msg.data[i].price);
  }
  else if(errno == ENOMSG)
  {
    perror("No victories message ");
  }
  else if(errno == EINVAL || errno == EIDRM)
  {
      perror("The queue doesn't exit or removed unexpectedly. ");
      exit(EXIT_FAILURE);
  }
  else
  {
      perror("Unexpected error on msgrcv.");
      exit(EXIT_FAILURE);
  }
}