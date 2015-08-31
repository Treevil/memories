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
*/

#include"header.h"

int main(int argc, char* argv[])
{

	char nome_ris[NAME_LEN];
	int id_shm, id_sem;
	int m;
	srand(time(NULL));
	sscanf(argv[1],"%s",nome_ris);
	sscanf(argv[2],"%d", &id_shm);
	sscanf(argv[3],"%d", &id_sem);
	printf("Agent for %s running!\n", nome_ris);
	P(id_sem, 0);
	bet *o = (bet *)shmat(id_shm, NULL, 0);
	if( (int) o == -1)
	{
		perror("Error shared memory attach...");
		V(id_sem, 0);
		exit(EXIT_FAILURE);
	}

	for(m=0; m<MAX_BET; m++)
	{
		(o+m)->pid_gambler = 10;
		(o+m)->quantity = 5;
		(o+m)->price_each = 122;
	}
   V(id_sem, 0);
	exit(EXIT_SUCCESS);
}