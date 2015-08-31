/*
Constant
-> MAX_GAMBLER     : Max number of gambler
-> MAX_RSC_GAMBLER : Max number of resource allowed for gambler
-> NAME_LEN        : Max lenght of names
-> MAX_BET         : Max bet on tbe same TAO
*/
#ifndef MAX_GAMBLER
    #define MAX_GAMBLER 10  
#endif
#ifndef MAX_RSC_GAMBLER
    #define MAX_RSC_GAMBLER 10
#endif
#ifndef NAME_LEN
    #define NAME_LEN 20
#endif
//tipo per le richieste di risorse da cliente a banditore
#ifndef RES_REQ_TYPE
  #define RES_REQ_TYPE 1554 
#endif
#ifndef MAX_BET
  #define MAX_BET 5
#endif


/*
#if defined(__GNU__LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
#else
	union semun{
		int val;
		struct semid_ds *buf;
		unsigned short int *array;
		struct seminfo *__buf;
	};
#endif


/
Library 
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
#ifndef __UNISTD_H__
    #include <unistd.h>
    #define __UNISTD_H__
#endif
#define SHMKEY 1554
#define SEMKEY 1555

/*
***********************
*  Data Structures    *
***********************
*/

/*
  resource   : Info about resource
  rsc_msg    : Message for exchange info about data 
*/
typedef struct {
    char name[NAME_LEN];
    int quantity;
    int price;
} resource;

typedef struct 
{
    char resource[NAME_LEN];
    int id_gambler[MAX_GAMBLER];
    int num_gambler;
} sorted_wish;


typedef struct {
    long type;
    int pid;
    int num_resource;
    resource data[MAX_RSC_GAMBLER];
} rsc_msg;


typedef struct{
    int pid_gambler;
    int num_resource;
    resource resource[MAX_RSC_GAMBLER];
} wish;

typedef struct 
{
    int pid_gambler;
    int quantity;
    int price_each;
} bet;

typedef struct 
{
    long type;
    int key_shm;
    int key_smp;
    resource resource;  
} msg_alert;

typedef struct 
{
    long type;
    int pid;
    resource rsc;
    
} msg_done;

/*
***************
*  Prototypes *
***************
*/


int load_resource(const char* file_name, resource data[], int* length);
int new_sharedmemory(int key);
int new_semaphore(int key);
int ipc_key_creation();
void detach_semaphore(int id_sem);
void detach_sharedMemory(int id_shm);