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
void gambler_registration(int id_queue, wish gambler_wish[], int* num_gambler_wish);
int alert_gambler(int num_gambler_wish, int id_smp, int id_shm,
                   int id_queue, wish gambler_wish[], 
                   resource rsc_owned);
int queue_remove(int id_queue);
void wish_sort(resource rsc_owned[], int num_rsc_owned, 
  wish gambler_wish[], int num_gambler_wish, sorted_wish wishbyname[]);

int main(int argc, char const *argv[])
{

  /*
    Data Structures:
      -> resource 
          -> rsc_owned      : Auctioneer Resource 
      -> Wish
         -> gambler_wish    : All request from gambler
      -> inìt  
       -> num_rsc_owned     : number auctioneer resource
       -> IPC_Key           : IPC key 
       -> key_queue         : key queue 
       -> num_gambler_wish  : total number of registred gambler
       -> i,j               : loop index 
  */

  resource rsc_owned[MAX_RSC_GAMBLER];
  wish gambler_wish[MAX_GAMBLER];
  int num_rsc_owned, num_gambler_wish, IPC_Key, i, j;
  int id_shm[MAX_RSC_GAMBLER], id_queue, id_smp[MAX_RSC_GAMBLER];
  int tao[MAX_RSC_GAMBLER];
  sorted_wish sorted_rsc[MAX_RSC_GAMBLER];



  int m;
  //bet *o;

  printf("\t\t*****************\n\t\t*   Auctioneer  *\n\t\t*****************\n\n\n\n");
  load_resource(argv[1], rsc_owned, &num_rsc_owned);

  printf("I have the following resource to sell:\n ");
  for(i=0; i < num_rsc_owned; i++)
      printf("-> %d %s for %d € each\n", rsc_owned[i].quantity, rsc_owned[i].name, rsc_owned[i].price);

  id_queue = queue_generation();
  printf("Press any key for start to accept bet for the gamblers \n");
  getchar();
  gambler_registration(id_queue, gambler_wish, &num_gambler_wish);

  wish_sort(rsc_owned, num_rsc_owned, gambler_wish, num_gambler_wish, sorted_rsc);
 
  for (i = 0; i < num_rsc_owned; i++) {
      printf("TAO for %s... \n", sorted_rsc[i].resource);
      printf("Preparation new shared memory area... ");
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

      sleep(3 * sorted_rsc[i].num_gambler);
      
      bet *o = (bet *)shmat(id_shm[i], NULL, 0);
      if((int) o == -1) 
      {
        perror("Error shared memory attach...");
        exit(EXIT_FAILURE);
      }

      printf("removing semaphore... ");
      detach_semaphore(id_smp[i]);
      printf("Done\n");
      printf("Removing shared memory... ");
      detach_sharedMemory(id_shm[i]);
      printf("Done\n");
      printf("TAO per %s deallocato\n", sorted_rsc[i].resource);
  }
  sleep(2);
  queue_remove(id_queue);

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
        sizeof(rsc_msg)-sizeof(long), RES_REQ_TYPE, IPC_NOWAIT);
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
  // Phase 1: Sort resource
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
  // Phase 2: Send a message for eliminate useless agent.
  // TODO


}













