/*
***************
*  CONSTANT   *
***************
*/
// Max number of Gambler
#ifndef MAX_GAMBLER
	#define MAX_GAMBLER 10 	
#endif
// Max lenght of names
#ifndef NAME_LEN
	#define NAME_LEN 20
#endif
// Max number of resource for gambler
#ifndef MAX_GAMBLER_RESOURCE
	#define MAX_GAMBLER_RESOURCE 10
#endif
#ifndef MAX_TAO_OF
	#define MAX_TAO_OF 5
#endif

/*
***************
*  Library    *
***************
*/

#ifndef __STDIO_H__
	#include <stdio.h>
	#define __STDIO_H__ 	
#endif
#ifndef __STDLIB_H__
	#include <stdlib.h>
	#define __STLIB_H__
#endif
#ifndef __STRING_H__
	#include <string.h>
	#define __STRING_H__ 	
#endif
#ifndef __SYSTYPES_H__
	#include <sys/types.h>
	#define __SYSTYPES_H__
#endif
#ifndef __SYSIPC_H__
	#include <sys/ipc.h>
	#define __SYSIPC_H__
#endif
#ifndef __SYSMSG_H__
	#include <sys/msg.h>
	#define __SYSMSG_H__
#endif
#ifndef __SYSSHM_H__
	#include <sys/shm.h>
	#define __SYSSHM_H__
#endif
#ifndef __SYSSEM_H__
	#include <sys/sem.h>
	#define __SYSSEM_H__
#endif
#ifndef __ERRNO_H__
	#include <errno.h>
	#define __ERRNO_H__
#endif
#ifndef __SIGNAL_H__
	#include <signal.h>
	#define __SIGNAL_H__
#endif

#define SHMKEY 1554

/*
***********************
*  Data Structures    *
***********************
*/


typedef struct {
	char name[NAME_LEN];
	int quantity;
	int price;
} resource;

typedef struct {
	//long type;
	int pid;
	int num_resource;
	resource data[MAX_GAMBLER_RESOURCE];
} msg;


typedef struct{
	int pid_gambler;
	resource resource[MAX_GAMBLER_RESOURCE];
	int num_resource;
} request;

typedef struct 
{
	int pid_gambler;
	int quantity;
	int price_each;
} bet;

typedef struct 
{
	long type;
	int shm_key;
	int smp_key;
	resource resource;	
} msgAlert;


/*
***************
*  Prototypes *
***************
*/


int load_resource(const char* file_name, resource data[], int* length);