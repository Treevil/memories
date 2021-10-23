/* header.h */

//numero massimo di clienti
#ifndef MAX_CLIENT
	#define MAX_CLIENT 11
#endif
//durata in secondi dell'attesa per le iscrizioni dei clienti alle aste
#ifndef TIME_FOR_REQUEST
	#define TIME_FOR_REQUEST 10
#endif
//numero massimo di offerte per un tao
#ifndef MAX_TAO_OF
	#define MAX_TAO_OF 5
#endif
//numero massimo di tao contemporanei
#ifndef MAX_TAO
	#define MAX_TAO 3
#endif
//durata in secondi dei tao
#ifndef TAO_LAST
	#define TAO_LAST 15
#endif
//numero massimo di risorse per cliente
#ifndef MAX_CLIENT_R
	#define MAX_CLIENT_R 10 	
#endif
//tipo per le richieste di risorse da cliente a banditore
#ifndef RES_REQ_TYPE
	#define RES_REQ_TYPE 1554	
#endif




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
#define SEMKEY 1555
#define TAO_S 0


/*
Data structures 


1 - Resource
*/

typedef struct
{
	char name[20];
	int quantity;
	int price;
} resource;

/*
2 - Structure for data passagge between 
	Banditore/Client and Client/Banditore 
*/
typedef struct
{
	long type;
	int pid;
	int num_res;
	resource data[MAX_CLIENT_R];
}msg1;


/*

3 - da banditore a cliente
struttura messaggio per la connessione ai tavoli delle offerte
*/
typedef struct
{
	long type;
	int shm_key;
	int smp_key;
	resource res;
}msg2;

/*
4 - Structure for request
*/

typedef struct
{
	int pid_applicant;
	resource resource[MAX_CLIENT_R];
	int num_res;
}request;

/*
5 - Strcutre for offer
*/

typedef struct
{
	int pid_offer;
	int quantity;
	int prize_each;
}offer;


// Prototypes

int band_load_resource(const char*,resource risorse[],int*);
void printTAO_resources(resource arr[],int n);
int client_load_resource(const char*,resource risorse[],int*,int*);
void print_client_resource(resource arr[],int);
int new_sharedMemory(int key);
int new_semaphore(int key);
int alert_clients(int queue,int shm,int sem,request ric[],
						int num_req,resource res);
int P(int semid, int semnum);
int V(int semid, int semnum);
int max_of(offer *o);
void deallocate_tao(int shmid);
void deallocate_semaphore(int semid);
void deallocate_queue(int qid);
void print_tao(offer *o);
int contains(resource ris_ric[],int num_ris,char ris[]);

