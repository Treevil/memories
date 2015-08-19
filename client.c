#include "header.h"

int main(int argc, char const *argv[])
{
    resource risorse[MAX_RIS];//max n risorse per ogni cliente
	int budget, num_ris,i;

	//lettura risorse
	if(client_load_resource(argv[1],risorse,&budget,&num_ris)<0)
	{
		perror("Error while opening the file...");
		exit(EXIT_FAILURE);
	}
	
	//stampa risorse
	printf("Ho %d€ e devo comprare:\n",budget);
	stampa_risorse_client(risorse,num_ris);


	int key;

	for(i=0;i<MAX_RIS;i++)
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
	return 0;
}



