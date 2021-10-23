#include "header.h"

int n, child[MAX_GAMBLER];

void handler_int(int s)
{
	int i;
	for(i=0;i<n;i++)
		kill(child[i],SIGKILL);
	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
	signal(SIGINT,handler_int);
	int i,num_gambler;
	char* args[5];
	sscanf(argv[1], "%d", &num_gambler);

	if (argc > 2 )
	{
		printf("Error!");
		exit(EXIT_FAILURE);
	}
	for(i=0; i< num_gambler; i++)
	{
	child[i]=fork();
	if(child[i]==0)
	{
	  char* args[] = {
	      "gambler.out",
	      "resource.txt",
	      NULL
	    };
		execve(args[0],args,NULL);
		perror("Execve error...");
	}
	else if(child[i]==-1)
		{
			perror("Error while generating client...");
			exit(EXIT_FAILURE);
		}
	}
	for(i=0; i<n; i++)
		wait(NULL);
		
}