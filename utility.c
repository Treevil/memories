// utility.c
#include "header.h"

/*
Used for load resouce from file. 
*/
int load_resource(const char* file_name, resource data[], int* length)
{
	// Open the file
	printf("Loading Resource....  ");
	FILE *fp = fopen(file_name, "r");
	if(!fp) //if something goes wrong...
	{
		printf("FAIL \n");
		perror("Error while opening the file. Resource cannot be loaded.\n");
		return -1;
	}

	// Load data
	*length=0;
	while(fscanf(fp,"%s",data[*length].name)!=EOF)
	{		
		fscanf(fp,"%d",&(data[*length].quantity));
		fscanf(fp,"%d",&(data[*length].price));
		fgetc(fp);
		(*length)++;
	}
	//Close the file
	if(fclose(fp)) {
		printf("WARNING\n");
		perror("Error while closing the file. Resource loaded anyway.\n");
		return -1; 
	}

	printf("Ok \n");
	return 0;
}

