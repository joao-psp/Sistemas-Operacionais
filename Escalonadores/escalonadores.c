/**
  Implementar:
  FCFS Simples  ok
  Round-Robin
  FCFS com prioridade sem preempção
  SJF ok
  Round-Robin com prioridade e envelhecimento
**/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"
#define T_MAX  50

enum state {NEW, READY, RUNNING, COMPLETED};

typedef struct process
{
   struct process *prev ;
   struct process *next ;
   int id ;
   int date_criacao;
   int data_conclusao;
   int duracao;
   int pri_dinamica;
   int pri_estatica;
   int tempo_executando;
   int tempo_quantum;
   enum state estado_atual;
} process ;

process *queue_novos = NULL;
process *queue_prontos = NULL;
process *queue_executando = NULL;
process *queue_finalizados = NULL;
int process_total = 0;
int current_time = 0;
int date_criacao = 0;
int duracao = 0;
int pri_estatica = 0;


struct process* get_process(process* queue, int id){
  process* first = queue;
  process* result = NULL;
  if(queue != NULL){
    do{
      if((*queue).id == id){
        result = queue;
        break;
      }
      else{
        queue = (*queue).next;
      }
    }while(queue != first);
  }
  return result;
}

 struct process* create_process(int id , int date_criacao, int duracao,
  int pri_estatica){

   struct process* new_process = (process*) malloc(sizeof(process));
   (*new_process).id = id;
   (*new_process).date_criacao = date_criacao;
   (*new_process).duracao = duracao;
   (*new_process).pri_dinamica = 0;
   (*new_process).pri_estatica = pri_estatica;
   (*new_process).tempo_executando = 0;
   (*new_process).tempo_quantum = 0;
   (*new_process).estado_atual = NEW;
 }

int init(){
  printf("Quantos processos quer simular?\n");
  scanf("%d", &process_total);
  for (int i = 0; i < process_total; i++) {
      printf("Entre com a info: tempo_criacao Duracao Prioridade:\n");
      scanf("%d %d %d", &date_criacao, &duracao, &pri_estatica);
      struct process* p = create_process(i, date_criacao, duracao, pri_estatica);
      queue_append((queue_t**)&queue_novos, (queue_t*)p);
  }
  return process_total;
}

void print_header(int n){
  printf("tempo   ");
  for (int i = 0; i < n; i++) {
    printf(" P%d ", i);
  }
  printf("\n");
}

void print_body(int t, int n){
   printf("%2d- %2d ", t, t+1);
   for (int i = 0; i < n; i++) {
     process *current = NULL;

     current = get_process(queue_novos, i);
     if(current != NULL) //processo na fila de novos
     {
       printf("    "); // não executa nada
     }

     current = get_process(queue_prontos, i);
     if(current != NULL) //processo na fila de prontos
     {
       printf("  --");
     }

     if(queue_executando != NULL && (*queue_executando).id == i) //processo executando
     {
       printf("  ##");
     }

     current = get_process(queue_finalizados, i);
     if(current != NULL) //processo na fila de prontos
     {
       printf("  --");
     }
   }
   printf("\n");
 }

void fcfs(int n){
  while(current_time < T_MAX){
    if(queue_executando != NULL){
      if((*queue_executando).duracao == (*queue_executando).tempo_executando){
        (*queue_executando).estado_atual = COMPLETED;
        (*queue_executando).data_conclusao = current_time;
        queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
        queue_executando = NULL;
      }
    }

    if(queue_size((queue_t*)queue_finalizados) < n)
    {
      process *aux = queue_novos;
      int count = queue_size((queue_t*)queue_novos);
      for (int i = 0; i < count; i++) {
        if((*aux).date_criacao == current_time){
          queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
          queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
          (*aux).estado_atual = READY;
          aux = queue_novos;
        }
        else{
          aux = (*aux).next;
        }
      }

      if(queue_executando == NULL)//processador ocioso
      {
        //pegar processo pronto se houver algum
        if(queue_size((queue_t*)queue_prontos) > 0){
          process *p = (process *)queue_remove((queue_t**)&queue_prontos, (queue_t*)queue_prontos);
          queue_executando = p;
          (*queue_executando).estado_atual = RUNNING;
          (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
        }
      }
      else
      {
        (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
      }
      print_body(current_time, n);
    }
    current_time++;
  }
}

void sjf(int n){

 while(current_time < T_MAX){
   if(queue_executando != NULL){
     if((*queue_executando).duracao == (*queue_executando).tempo_executando){
       (*queue_executando).estado_atual = COMPLETED;
       (*queue_executando).data_conclusao = current_time;
       queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
       queue_executando = NULL;
     }
   }

   if(queue_size((queue_t*)queue_finalizados) < n)
   {
     process *aux = queue_novos;
     int count = queue_size((queue_t*)queue_novos);
     for (int i = 0; i < count; i++) {
       if((*aux).date_criacao == current_time){
         queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
         queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
         (*aux).estado_atual = READY;
         aux = queue_novos;
       }
       else{
         aux = (*aux).next;
       }
     }

     if(queue_executando == NULL){//processador ocioso{
       //pegar processo pronto se houver algum
       if(queue_size((queue_t*)queue_prontos) > 0){
         process *aux = queue_prontos;
         process *aux2 = queue_prontos;
         int count = queue_size((queue_t*)queue_prontos);
         for(int i=0; i < count;i++){
             if(aux->duracao > aux2->duracao){
               aux = aux2;
               aux2 = aux2->next;
             }else{
               aux2 = aux2->next;
             }
         }
         process *p = (process *)queue_remove((queue_t**)&queue_prontos, (queue_t*)aux);
         queue_executando = p;
         (*queue_executando).estado_atual = RUNNING;
         (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       }
     }
     else
     {
       (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
     }
     print_body(current_time, n);
   }
   current_time++;
 }
}

void pri_sem_preemp(int n){

  while(current_time < T_MAX){
   if(queue_executando != NULL){
     if((*queue_executando).duracao == (*queue_executando).tempo_executando){
       (*queue_executando).estado_atual = COMPLETED;
       (*queue_executando).data_conclusao = current_time;
       queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
       queue_executando = NULL;
     }
   }

   if(queue_size((queue_t*)queue_finalizados) < n)
   {
     process *aux = queue_novos;
     int count = queue_size((queue_t*)queue_novos);
     for (int i = 0; i < count; i++) {
       if((*aux).date_criacao == current_time){
         queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
         queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
         (*aux).estado_atual = READY;
         aux = queue_novos;
       }
       else{
         aux = (*aux).next;
       }
     }

     if(queue_executando == NULL){//processador ocioso{
       //pegar processo pronto se houver algum
       if(queue_size((queue_t*)queue_prontos) > 0){
         process *aux = queue_prontos;
         process *aux2 = queue_prontos;
         int count = queue_size((queue_t*)queue_prontos);

         for(int i=0; i < count;i++){
             if(aux->pri_estatica > aux2->pri_estatica){
               aux = aux2;
               aux2 = aux2->next;
             }else{
               aux2 = aux2->next;
             }
         }
         process *p = (process *)queue_remove((queue_t**)&queue_prontos, (queue_t*)aux);
         queue_executando = p;
         (*queue_executando).estado_atual = RUNNING;
         (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       }
     }
     else
     {
       (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
     }
     print_body(current_time, n);
   }
   current_time++;
  }

}

void pri_com_preem(int n){

  while(current_time < T_MAX){
   if(queue_executando != NULL){
     if((*queue_executando).duracao == (*queue_executando).tempo_executando){
       (*queue_executando).estado_atual = COMPLETED;
       (*queue_executando).data_conclusao = current_time;
       queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
       queue_executando = NULL;
     }else{
       queue_executando->estado_atual = READY;
       queue_append((queue_t**)&queue_prontos, (queue_t*)queue_executando);
       queue_executando = NULL;
     }
   }

   if(queue_size((queue_t*)queue_finalizados) < n)
   {
     process *aux = queue_novos;
     int count = queue_size((queue_t*)queue_novos);
     for (int i = 0; i < count; i++) {
       if((*aux).date_criacao == current_time){
         queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
         queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
         (*aux).estado_atual = READY;
         aux = queue_novos;
       }
       else{
         aux = (*aux).next;
       }
     }

     if(queue_executando == NULL){//processador ocioso{
       //pegar processo pronto se houver algum
       if(queue_size((queue_t*)queue_prontos) > 0){
         process *aux = queue_prontos;
         process *aux2 = queue_prontos;
         int count = queue_size((queue_t*)queue_prontos);

         for(int i=0; i < count;i++){
             if(aux->pri_estatica > aux2->pri_estatica){
               aux = aux2;
               aux2 = aux2->next;
             }else{
               aux2 = aux2->next;
             }
         }
         process *p = (process *)queue_remove((queue_t**)&queue_prontos, (queue_t*)aux);
         queue_executando = p;
         (*queue_executando).estado_atual = RUNNING;
         (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       }
     }
     else
     {
       (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
     }
     print_body(current_time, n);
   }
   current_time++;
  }

}

void rr_quantum2_sem_prio(int n){

  while(current_time < T_MAX){
   if(queue_executando != NULL){
     if((*queue_executando).duracao == (*queue_executando).tempo_executando){
       (*queue_executando).estado_atual = COMPLETED;
       (*queue_executando).data_conclusao = current_time;
       queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
       queue_executando = NULL;
     }else{
       if(queue_executando->tempo_quantum == 2){
         queue_executando->tempo_quantum = 0;
         queue_executando->estado_atual = READY;
         queue_append((queue_t**)&queue_prontos, (queue_t*)queue_executando);
         queue_executando = NULL;
       }
     }
   }

   if(queue_size((queue_t*)queue_finalizados) < n)
   {
     process *aux = queue_novos;
     int count = queue_size((queue_t*)queue_novos);
     for (int i = 0; i < count; i++) {
       if((*aux).date_criacao == current_time){
         queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
         queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
         (*aux).estado_atual = READY;
         aux = queue_novos;
       }
       else{
         aux = (*aux).next;
       }
     }

     if(queue_executando == NULL){//processador ocioso{
       //pegar processo pronto se houver algum
       if(queue_size((queue_t*)queue_prontos) > 0){

         queue_executando = (process*) queue_remove((queue_t **) &queue_prontos, (queue_t *) queue_prontos);
         queue_executando->tempo_quantum++;
         (*queue_executando).estado_atual = RUNNING;
         (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       }
     }
     else
     {
       (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       queue_executando->tempo_quantum++;
     }
     print_body(current_time, n);
   }
   current_time++;
  }

}

void rr_quantum2_env(int n ){
  while(current_time < T_MAX){
   if(queue_executando != NULL){
     if((*queue_executando).duracao == (*queue_executando).tempo_executando){
       (*queue_executando).estado_atual = COMPLETED;
       (*queue_executando).data_conclusao = current_time;
       queue_append((queue_t**)&queue_finalizados, (queue_t*)queue_executando);
       queue_executando = NULL;
     }else{
       if(queue_executando->tempo_quantum == 2){
         queue_executando->tempo_quantum = 0;
         queue_executando->estado_atual = READY;
         queue_append((queue_t**)&queue_prontos, (queue_t*)queue_executando);
         queue_executando = NULL;
       }
     }
   }

   if(queue_size((queue_t*)queue_finalizados) < n)
   {
     process *aux = queue_novos;
     int count = queue_size((queue_t*)queue_novos);
     for (int i = 0; i < count; i++) {
       if((*aux).date_criacao == current_time){
         queue_remove((queue_t**)&queue_novos, (queue_t*)aux);
         queue_append((queue_t**)&queue_prontos, (queue_t*)aux);
         (*aux).estado_atual = READY;
         aux = queue_novos;
       }
       else{
         aux = (*aux).next;
       }
     }

     if(queue_executando == NULL){//processador ocioso{
       //pegar processo pronto se houver algum
       if(queue_size((queue_t*)queue_prontos) > 0){

         process *aux = queue_prontos;
         process *aux2 = queue_prontos;
         int count = queue_size((queue_t*)queue_prontos);

         for(int i=0; i < count;i++){
             if((aux->pri_estatica - aux->pri_dinamica) > (aux2->pri_estatica - aux2->pri_dinamica)){
               aux = aux2;
               aux2 = aux2->next;
             }else{
               aux2 = aux2->next;
             }
         }
         process *p = (process *)queue_remove((queue_t**)&queue_prontos, (queue_t*)aux);
         queue_executando = p;

         queue_executando->tempo_quantum++;
         (*queue_executando).estado_atual = RUNNING;
         (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       }
     }
     else
     {
       (*queue_executando).tempo_executando = (*queue_executando).tempo_executando + 1;
       queue_executando->tempo_quantum++;
     }
     print_body(current_time, n);
   }
      // envelhecendo processos :
      process *aux = queue_prontos;
      int count = queue_size((queue_t*)queue_prontos);
      for (int i = 0; i < count; i++) {
        aux->pri_dinamica++;
        aux = aux->next;
      }

   current_time++;
  }
}

int main(int argc, char const *argv[]) {
  int n;
  n = init();
  print_header(n);
  rr_quantum2_env(n);
  return 0;
}
