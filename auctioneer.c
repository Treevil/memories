#include "header.h"

int main(int argc, char const *argv[])
{
/*
  Data Structures:
    -> resource 
        -> rsc_owned       : auctioneer resource 
    -> Wish
       -> gambler_wish
    -> inìt  
     -> num_rsc_owned     : number auctioneer resource
     -> IPC_Key           : IPC key 
     -> key_queue         : key queue 
     -> num_gambler_wish  : total number of registred gambler
     -> i,j               : loop index 
*/

    resource rsc_owned[MAX_RSC_GAMBLER];
    wish gambler_wish[MAX_GAMBLER];
    int num_rsc_owned, num_gambler_wish, key_queue, IPC_Key, i, j;

    /*
    ->request
      -> num__gambler: total number of registred gambler
    */


    printf("\t\t*****************\n\t\t*   Auctioneer  *\n\t\t*****************\n\n\n\n");
    load_resource(argv[1],rsc_owned, &num_rsc_owned);

    printf("I have the following resource to sell:\n ");
    for(i=0; i < num_rsc_owned; i++)
        printf("-> %d %s for %d € each\n", rsc_owned[i].quantity, rsc_owned[i].name, rsc_owned[i].price);

    printf("Creation IPC Key... ");
    if((IPC_Key = ftok("gambler.c", 'G')) == -1)
    {
        perror("FAIL. Error on ftok...");
        exit(EXIT_FAILURE);
    }
    printf("Done\n");

    printf("Creation messagge queue... ");
    key_queue = msgget(IPC_Key, 0666|IPC_CREAT);
    if(key_queue == -1)
    {
        perror("FAIL. Error on msgget.");
        exit(EXIT_FAILURE);
    }
    printf("Done\n");


    printf("Press any key for start to accept bet for the gamblers \n");
    getchar();

    printf("Registration phase... On going\n");
    int timer = 3; // Waiting secs for requests
    rsc_msg recived_msg;
    num_gambler_wish = 0;
    while(timer > 0)
    {
      rsc_msg recived_msg;
      // FIFO message queue
      int rcv_bytes = msgrcv(key_queue, &recived_msg, sizeof(rsc_msg)-sizeof(long), RES_REQ_TYPE, IPC_NOWAIT);
      if(rcv_bytes > 0)
      {
        gambler_wish[num_gambler_wish].pid_gambler = recived_msg.pid;
        gambler_wish[num_gambler_wish].num_resource = recived_msg.num_resource;
        for(i = 0; i < recived_msg.num_resource; i++)
          gambler_wish[num_gambler_wish].resource[i] = recived_msg.data[i];
        num_gambler_wish++;
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

    printf("Start DEBUG:\n");
    printf("%d\n", num_rsc_owned);
    
    for(i = 0; i < num_gambler_wish; i++) {
      printf("PID: %d\n", gambler_wish[i].pid_gambler );
      printf("Numer of Resource: %d\n", gambler_wish[i].num_resource);
      for(int j = 0; j < gambler_wish[i].num_resource; j++)
            printf("-> %d %s for %d€ each\n",gambler_wish[i].resource[j].quantity, gambler_wish[i].resource[j].name, gambler_wish[i].resource[j].price);
      
    }

    // Creation Shared Memory Space 
    for (i = 0; i < num_rsc_owned; i++) {
      printf("TAO for %s... ", rsc_owned[i].name);
      printf("Preparation new shared memory area... ");
      int id_shm = new_sharedmemory(SHMKEY+(i*2));
      printf("Done\n");
      printf("Preparation new semaphore... ");
      int id_smp = new_semaphore(SEMKEY+(2*i));//crea semaforo
      printf("Done\n");
      
      // Avviso i Gamblers 
      printf("\n\nSend message to interessed gamblers.... ");
      msg_alert alert;
      for (j = 0;  j < num_gambler_wish; j++){
        for (int x; x < gambler_wish[j].num_resource; x++)
          if (strcmp(gambler_wish[j].resource[x].name, rsc_owned[i].name))
          {
            alert.type = gambler_wish[i].pid_gambler;
            alert.key_shm = id_shm;
            alert.key_smp = id_smp; 
            alert.resource = rsc_owned[i];
            while(msgsnd(key_queue, &alert, sizeof(msg_alert) - sizeof(long), 0) == -1){
              perror("Trying to send a message to gamblers\n");
              sleep(1);
            }
          }
      }
      printf("Done\n");
    }


    return 0;
}