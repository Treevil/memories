/*
Specification:
The process Auctioneer has the task of assigning resources 
to processes customers. The resources to be allocated are read from files.
For each resource, the file must indicate how many resources are actually 
available, and what is the minimum cost per unit. 
For example, following the pattern: <resource-name; units available; cost per unit>.

The Auctioneer creates a "tavolo delle offerte" (TAO) dedicated to that resource. 
The TAO is a shared memory area that contains the following information for 
each offer made by a gambler: pid-gamblers, units required, bet.

This process can create max 3 TAO simultaneously; if there are more resources to be
allocated, the Auctioneer will have to worry about deallocate the TAO no longer 
needed, and then a new one with allocating resources not yet assigned.

Each TAO may contain a limited number of deals (eg. 5). If the number of bids is 
higher than the available slots, only the very best offers are retained.

The Auctioneer keeps a list of customers registered to the system. For each customer, 
the auctioneer stores resources that the customer is interested. When the Auctioneer
opens a new TAO for a particular resource, the Auctioneer informs all customers 
interested in that resource by sending them, via message queues, corresponding 
to the id of shm TAO and the id of a semaphore that will adjust the time opening 
auction (we recommend using the waiting zero), along with the starting price at auction.

For each TAO, the Auctioneer handles the auction as follows:
After informing all interested gamblers, the TAO will start a timer to 3 seconds. 
After that it will launch an auction, and then allow customers to make their offerings.
The auction may take a while, depending on function the number of customers affected 
(the implementation of this part is free). 
When the time expires, the auctioneer closes the auction, reads the contents of the TAO 
and allocates resources according to the best bargains.
The allocation phase is made by send a message to customers that have 
acquired the resources.

@Author: Trevor Devalle
*/



#include "header.h"
// prototypes 
int queue_generation();
void gambler_registration(int id_queue, wish gambler_wish[], 
  int* num_gambler_wish);
int alert_gambler(int num_gambler_wish, int id_smp, int id_shm,
                   int id_queue, wish gambler_wish[], 
                   resource rsc_owned);
int queue_remove(int id_queue);
void wish_sort(resource rsc_owned[], int num_rsc_owned, 
  wish gambler_wish[], int num_gambler_wish, sorted_wish wishbyname[]);
/*agent_stop(int id_queue, wish gambler_wish, int num_gambler_wish, 
  sorted_wish sorted_rsc);*/
void getwinners(int id_shm, int id_sem, bet winners[]);
void log_winners(bet winners[], int num_rsc_owned, char file_name[]);

int main(int argc, char const *argv[])
{

  /*
    Data Structures:
      -> resource 
          -> rsc_owned      : Auctioneer Resource 
      -> Wish
         -> gambler_wish    : All request from gambler
      -> init  
       -> num_rsc_owned     : number auctioneer resource
       -> key_queue         : key queue 
       -> num_gambler_wish  : total number of registred gambler
       -> i,j               : loop index 
  */

  resource rsc_owned[MAX_RSC_GAMBLER];
  wish gambler_wish[MAX_GAMBLER];
  int num_rsc_owned, num_gambler_wish, i, j;
  int id_shm[MAX_RSC_GAMBLER], id_queue, id_smp[MAX_RSC_GAMBLER];
  int tao[MAX_RSC_GAMBLER];
  sorted_wish sorted_rsc[MAX_RSC_GAMBLER];
  int pid_process[MAX_RSC_GAMBLER];


  int m;
  bet winners[MAX_BET];

  printf("\t\t*****************\n\t\t*   Auctioneer  *\n\t\t*****************\n\n\n\n");
  load_resource(argv[1], rsc_owned, &num_rsc_owned, 1);

  printf("I have the following resource to sell:\n ");
  for(i=0; i < num_rsc_owned; i++)
      printf("-> %d %s for %d € each\n", rsc_owned[i].quantity, rsc_owned[i].name, rsc_owned[i].price);

  id_queue = queue_generation();
  printf("Press any key for start to accept bet for the gamblers \n");
  getchar();
  gambler_registration(id_queue, gambler_wish, &num_gambler_wish);

  wish_sort(rsc_owned, num_rsc_owned, gambler_wish, num_gambler_wish, sorted_rsc);
  // TODO: Kill useless agent
  //agent_stop(id_queue, gambler_wish, num_gambler_wish, sorted_rsc);
  int activetao = 0;
  for (i = 0; i < num_rsc_owned; i++) {
      activetao++;

      pid_process[i] = fork();
      if (pid_process[i] == -1){
        perror("Eror on Fork procedure... ");
        exit(EXIT_FAILURE);
      }
      else if(!pid_process[i]) {
        printf("Creation new %s shared memory area... ", sorted_rsc[i].resource);
        id_shm[i] = new_sharedmemory(SHMKEY + (i * 2) );
        printf("Done\n");
        printf("Preparation new semaphore... ");
        id_smp[i] = new_semaphore(SEMKEY + (2 * i) ); 
        printf("Done\n");

        msg_alert alert;
        for (j = 0; j < sorted_rsc[i].num_gambler; j++){
          alert.type = sorted_rsc[i].id_gambler[j];
          alert.key_shm = id_shm[i];
          alert.key_smp = id_smp[i]; 
          alert.resource = rsc_owned[i];
          printf("Send alert message for %s... \n", sorted_rsc[i].resource);
          while(msgsnd(id_queue, &alert, sizeof(msg_alert) - sizeof(long), 0) == -1){
            perror("Trying to send a message to gamblers\n");
            sleep(1);
          }
          printf("Messages sended!\n");
        }

        sleep(3 + sorted_rsc[i].num_gambler);
        
        getwinners(id_shm[i], id_smp[i], winners);
        
        

        printf("Removing %s shared memory... ", sorted_rsc[i].resource);
        P(id_smp[i], 0);
        detach_sharedMemory(id_shm[i]);
        V(id_smp[i], 0);
        printf("Done\n");

        printf("removing semaphore... ");
        detach_semaphore(id_smp[i]);
        printf("Done\n");
        log_winners(winners, sorted_rsc[i].num_gambler, sorted_rsc[i].resource);
        exit(EXIT_SUCCESS);
      }
      else {
        if (activetao >= 3)
        {
          // Wait the end of any child process
          wait(0);
          activetao--;
        }
      }
    }

    // Wait the end of all childs
    for (i = 0; i < num_rsc_owned; i++) {
      wait(NULL);
    }

    sleep(2);
    
    printf("\n\nSend victories message...\n\n");
    //send_victory(id_queue, rsc_owned, num_rsc_owned);
    printf("Done\n");
    // TODO:
    // Mandare ad ogni processo le vincite

    sleep(3);
    queue_remove(id_queue);

    printf("\n\nEnd ;)\n");

  return 0;

}

/*
 Function for register gambler wish into the auctionner 
 process
*/
void gambler_registration(int id_queue, wish gambler_wish[], int* num_gambler_wish) {
    printf("Gambler registration.... \n");
    int timer = 3, i; // Waiting secs for requests
    rsc_msg recived_msg;
    *num_gambler_wish = 0;

    while(timer > 0)
    {
      rsc_msg recived_msg;
      // FIFO message queue
      int rcv_bytes = msgrcv(id_queue, &recived_msg, 
        sizeof(rsc_msg) - sizeof(long), RES_REQ_TYPE, IPC_NOWAIT);
      if(rcv_bytes > 0)
      {
        gambler_wish[*num_gambler_wish].pid_gambler = recived_msg.pid;
        gambler_wish[*num_gambler_wish].num_resource = recived_msg.num_resource;
        for(i = 0; i < recived_msg.num_resource; i++)
          gambler_wish[*num_gambler_wish].resource[i] = recived_msg.data[i];
        (*num_gambler_wish)++;
        printf("Riceved registration from: %d\n", recived_msg.pid);
      }
      else if(errno == ENOMSG)
      {
          printf("No registration in the queue... Time left': %d\n", timer);
          sleep(1);
          timer--;
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

    printf("Enrollment completed! \n");
}

int queue_generation() {
  int IPC_Key, id_queue;

  IPC_Key = ipc_key_creation();
  printf("Creation messagge queue... ");
  id_queue = msgget(IPC_Key, 0666|IPC_CREAT);
  if(id_queue == -1)
  {
    perror("FAIL. Error on msgget.");
    exit(EXIT_FAILURE);
  }
  printf("Done\n");
  
  return id_queue;
}


int queue_remove(id_queue){
  printf("Remove message queue...  ");
  if (msgctl(id_queue, IPC_RMID, NULL)==-1)
  {
    perror("Error while removing queue...");
    exit(EXIT_FAILURE);
  }
  printf("Done\n");
  return 0;
}


/*
From Gambler:{res1,res2,res3,etc...}
to Res1{Pid1, Pid2, etc}

*/
void wish_sort(resource rsc_owned[], int num_rsc_owned, wish gambler_wish[], 
  int num_gambler_wish, sorted_wish wishbyname[]){
  int index,i, k, j;
  for (i = 0; i < num_rsc_owned; i++){
    strcpy(wishbyname[i].resource, rsc_owned[i].name);
    index = 0;
    for (j = 0; j < num_gambler_wish; j++){
      for (k = 0; k < gambler_wish[j].num_resource; k++){
        if (!strcmp(wishbyname[i].resource, gambler_wish[j].resource[k].name)){
          wishbyname[i].id_gambler[index] = gambler_wish[j].pid_gambler;
          index++;
        }
      }
    }
    wishbyname[i].num_gambler = index;
  }
}

/*
Stop the creation of agent for unowned resouces

void agent_stop(int id_queue, wish gambler_wish,
 int num_gambler_wish, resource rsc_owned, int num_rsc_owned)
{
  int i, j, g;
  msg_alert alert;
  // For each request
  for (i = 0; i < num_gambler_wish; i++){
    // For each resource inside the request
    for (j = 0; j < gambler_wish[i].num_resource; j++){
      done = g = 0;
      while (g < num_rsc_owned && done != 1){ 
        if (strcmp(gambler_wish.resource[j].name, rsc_owned.name))
          g++:
        else 
          done = 1;
      }
      if (done == 0) {

        /*
        alert.type = sorted_rsc[i].id_gambler[j];
        alert.key_shm = 0;
        alert.key_smp = 0; 
        alert.resource = rsc_owned[i];
        printf("Send alert message for %s... \n", sorted_rsc[i].resource);
        while(msgsnd(id_queue, &alert, sizeof(msg_alert) - sizeof(long), 0) == -1){
          perror("Trying to send a message to gamblers\n");
          sleep(1);
        }
        printf("Messages sended!\n");
        *

      }
    }
}
/*
  Init the data structure to the value saved
  into the shared memory
*/

void getwinners(int id_shm, int id_sem, bet winners[]) {
  int i;
  bet *o = (bet *)shmat(id_shm, NULL, 0);
  P(id_sem,0);
  if( (int) o == -1)
  {
    perror("Error shared memory attach...");
    V(id_sem, 0);
    exit(EXIT_FAILURE);
  }

  for(i = 0; i < MAX_BET; i++)
  {
    winners[i].pid_gambler = (o + i)->pid_gambler;
    winners[i].quantity = (o + i)->quantity;
    winners[i].price_each = (o + i)->price_each;
  }
  V(id_sem, 0);
}

void log_winners(bet winners[], int num_rsc_owned, char file_name[]) {
  int i; 
  FILE *ofp;
  ofp = fopen(strcat(file_name,".log"), "w");
  if(!ofp) //if something goes wrong...
  {
    perror("Error while opening the file. Log cannot be created.\n");
    return; 
  }

  for(i = 0; i < num_rsc_owned; i++){
    if ( winners[i].pid_gambler != -1)
    {
      fprintf(ofp, "%d %d %d\n", winners[i].pid_gambler, winners[i].quantity,
       winners[i].price_each);
      fgetc(ofp);
    } 
    else
      break; 
  }
  fclose(ofp);

}

/*
int send_victory(int id_queue, resource rsc_wished[], int num_rsc_wished) {
  int i, j, length;
  FILE *f[MAX_RSC_GAMBLER];
  wish logs[MAX_GAMBLER];  
  int pids[MAX_GAMBLER], quantity[MAX_GAMBLER], price[MAX_GAMBLER]; 
  for (i = 0; i < num_rsc_owned; i++){
    if(!f[i] = fopen(strcat(rsc_owned[i],".log"), "w")) //if something goes wrong...
    {
      perror("Error while opening the file. Log cannot be created.\n");
      return -1; 
    }

    length = 0;
    while(fscanf(f,"%d", &pids[length])!=EOF)
    {
      fscanf(f,"%d", &quantity[length]);    
      fscanf(f,"%d", &price[length]);
      fgetc(f);
      length++;
    }

    for (m = 0; m < num_gambler_wish ; m++) {
      for (j = 0; j < length; j++)
        if (pid[j] == gambler_wish[m].pid_gambler) {
            wish_rsc_index = 0;
            while (wish_rsc_index < gambler_wish[m].num_resource) {
              if(!strcmp(rsc_owned[i], gambler_wish[m].resource[wish_rsc_index].name)) {
                gambler_wish[m].resource[wish_rsc_index].quantity = quantity;
                gambler_wish[m].resource[wish_rsc_index].price = price;
              }
            }
        }
    }

  }


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
}

*/




