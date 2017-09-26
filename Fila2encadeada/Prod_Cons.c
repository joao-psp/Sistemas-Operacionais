#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "queue.h"


#define CONT 10

int  counter;

typedef struct queue{
   struct queue *prev ; 
   struct queue *next ; 
   int    item ;
} queue ;

queue *buffer;
sem_t s_vaga, s_item, s_buffer;

pthread_t tid;      
pthread_attr_t attr; 


queue *initEle(queue *aux){
	
	int item = rand() % 1000;

	aux = (queue*) malloc (sizeof (queue));
    aux->item = item;
    aux->prev = NULL;
    aux->next = NULL;

    return aux;

}
void *produtor(void *id) {
   while (1){

	int pause = rand() % 4;
	sleep (pause);

	sem_wait(&s_vaga);
	sem_wait(&s_buffer);

	
	queue *novo;
	novo =initEle(novo);
	queue_append((queue_t **) &buffer, (queue_t*) novo);


	sem_post(&s_buffer);
	sem_post(&s_item);


//      print item
	printf("P%ld produziu %d\n",((long)id)+1,novo->item);
	}

	pthread_exit (NULL) ;
}

// consumidor Thread
void *consumidor(void *id) {
   while (1){

	sem_wait(&s_item);
	sem_wait(&s_buffer);


	queue *velho;
        velho = buffer;
	queue_remove ((queue_t**) &buffer, (queue_t*) velho);

	int item = velho->item;


	sem_post(&s_buffer);
	sem_post(&s_vaga);


	printf("\t\t\tC%ld consumiu %d\n",((long)id)+1,item);

	int pause = rand() % 4;
	sleep (pause);
   }

   pthread_exit (NULL) ;
}

void init() {

	counter = 0;
	buffer = NULL;

	sem_init(&s_buffer, 0, 1);
	sem_init(&s_vaga, 0, 5);
	sem_init(&s_item, 0, 0);

	// Atributos default
	pthread_attr_init (&attr) ;
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) ;

}



int main() {

	// Inicializando
	init();
	long i, status;
	pthread_t prod [3];
	pthread_t cons [3];

	// Criando Threads produtoras 
	for(i = 0; i < 3; i++) {
	// Create the thread
		status = pthread_create(&prod[i],&attr,produtor,(void *) i);
	}

	// Criando Threads consumidoras
	for(i = 0; i < 2; i++) {
		status = pthread_create(&cons[i],&attr,consumidor, (void *) i);
	}

	sleep(10);


	for (i=0; i<3; i++){
		printf ("Aguardando thread p%ld\n", i);
		status = pthread_join (prod[i], NULL) ;
		if (status){
			perror ("pthread_join") ;
			exit (1) ;
		}
	}

	for (i=0; i<2; i++){
		printf ("Aguardando thread c%ld\n", i);
		status = pthread_join (cons[i], NULL);
		if (status){
			perror ("pthread_join") ;
			exit (1) ;
		}
	}

	return 0;

}