#include "header.h"

int main(int argc, char const *argv[])
{
    resource resource[MAX_CLIENT_R];//max n risorse per ogni cliente
	int budget, num_res,i;
	int n[MAX_CLIENT_R];


	//lettura risorse
	if(client_load_resource(argv[1],resource,&budget,&num_res)<0)
	{
		perror("Error while the client is opening the file...");
		exit(EXIT_FAILURE);
	}
	
	//stampa risorse
	printf("I have %d € and I have to buy:\n",budget);
	print_client_resource(resource,num_res);


	int key;

	for(i=0;i<MAX_CLIENT_R;i++)
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



