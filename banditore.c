/* 

File: Banditore
@Authors: Trevor Devalle, Marco Davi
 
 */

#include "header.h"


int main(int argc, char const *argv[])
{
	//max n resource each client 
	resource band_resources[MAX_RIS*MAX_CLIENT];
	int num_resources = 0;

	// reading resource
	if(band_load_resource("resource.txt", band_resources, &num_resources) < 0){
		perror("Error while opening the file \n");
		exit(EXIT_FAILURE);
	}

	// print resource
	printf("Resources loaded:");
	printTAO_resources(band_resources,num_resources);

	return 0;
}