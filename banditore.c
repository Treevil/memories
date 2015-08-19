/* 

File: Banditore
@Authors: Trevor Devalle, Marco Davi
 
 */

#include "header.h"


int main(int argc, char const *argv[])
{
	printf("Banditore Started...\n\n\n");
	//max n resource each client 
	resource band_resources[MAX_RIS*MAX_CLIENT];
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
        perror("msgget error");
        exit(EXIT_FAILURE);
    }

	printf("Premere un tasto per leggere le richieste ");
	getchar();
	printf("Banditore in attesa di richieste...\n");


	return 0;
}


