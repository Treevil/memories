/*
Specification:
The process Agent is created by the gambler and has the task of acquiring a certain 
amount of resources.  The agent receives takes as arguments the amount of resources that 
have to acquire, the maximum budget for that resource and also the information needed to access 
the TAO, which the auction, the id of the shared memory segment and semaphore id required.

The access to the TAO must take place in the following way:
1) In the first 3 seconds, all agents just wait.
2) when the auction opens, agents mutually exclusive access to the TAO, and write a line of TAO their offer.
3) an agent can write an entry of TAO only under two conditions:
a. the entry is empty
b. The entry contains a lower bet than the one the agent are going to do
4) The angents can keep in the TAO 1 bet only.
5) The agents "compete" in the sense that continue to access the TAO until the auction closes. 
Every time you access the TAO verify that their offer is still among the best. If so release the TAO,
if not make a new offer to be at least better than the worst currently in TAO (provided that their budget allows them to increase the offer).

When the auction close, the process ends as well. 
*/

#include"header.h"

int main(int argc, char* argv[])
{

	char nome_ris[NAME_LEN];
	int id_shm, id_sem, id_queue, min_price, quantity, budget;
	int m;
	int done = 0;
	bet temp[MAX_BET];
	
	sscanf(argv[1], "%s", nome_ris);
	sscanf(argv[2], "%d", &id_shm);
	sscanf(argv[3], "%d", &id_sem);
	sscanf(argv[4], "%d", &min_price);
	sscanf(argv[5], "%d", &quantity);
	sscanf(argv[6], "%d", &budget);	
	sscanf(argv[7], "%d", &id_queue);	

	printf("Agent for %s is running!\n", nome_ris);
	P(id_sem, 0);
	
	bet *o = (bet *)shmat(id_shm, NULL, 0);
	if( (int) o == -1)
	{
		perror("Error shared memory attach...");
		V(id_sem, 0);
		exit(EXIT_FAILURE);
	}
	int i, j;
	// am I the first?
	if((o + 0) -> pid_gambler != getppid()) {
		printf("Im in!");
		// Have I enough budget?
		if ( budget > (o + 0)->price_each) {
			// Do I have already done a bet? If so, delete it!
			for (i = 0; i < MAX_BET -1 ; i++){
				if((o + i) -> pid_gambler == getppid()) {
					int index = 0;
					for (j = i + 1; j < MAX_BET; j++){
						temp[index].pid_gambler = (o + j)->pid_gambler;
						temp[index].quantity = (o + j)->quantity;
						temp[index].price_each = (o + j)->price_each;
					}
					for (j = i; i < MAX_BET - 1; i++){
						(o + j)->pid_gambler = temp[i].pid_gambler ;
						(o + j)->quantity = temp[i].quantity ;
						(o + j)->price_each = temp[i].price_each;
					}
					(o + MAX_BET)->pid_gambler = -1;
					(o + MAX_BET)->quantity = 0;
					(o + MAX_BET)->price_each = 0;
				}
				// Add new entry
				for (i = 0; i < MAX_BET; i++){
					temp[i].pid_gambler = (o + i)->pid_gambler;
					temp[i].quantity = (o + i)->quantity;
					temp[i].price_each = (o + i)->price_each;
				}

				for (i = 0; i < MAX_BET - 1; i++){
					(o + i + 1)->pid_gambler = temp[i].pid_gambler ;
					(o + i + 1)->quantity = temp[i].quantity ;
					(o + i + 1)->price_each = temp[i].price_each;
				}
				(o + 0)->pid_gambler = getppid();
				(o + 0)->quantity = quantity;
				
				if ((o + 0)->price_each == 0)
					(o + 0)->price_each	 = min_price;
				else
					(o + 0)->price_each	 = (o + 0)->price_each + 1;
			}
		}
	}
   V(id_sem, 0);
   // TODO: Wait Killer Message()
	exit(EXIT_SUCCESS);
}
