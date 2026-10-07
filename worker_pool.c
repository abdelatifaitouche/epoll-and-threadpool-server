#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct task_queue{
	int *tasks ; // socket filedescriptors
	
	pthread_mutex_t m ; 
	pthread_cond_t cond_empty ;
	pthread_cond_t cond_full ; 

	uint capacity ; 
	int head ; 
	int tail ;
	int length ; 
}task_queue_t ; 


task_queue_t *new_queue(uint capacity){
	task_queue_t *q = (task_queue_t*)malloc(sizeof(task_queue_t)) ;

	if(!q){
		perror("Error init a task queue") ;  
		return NULL ; 
	}
	
	q->capacity = capacity ;

	int *tasks = (int*)calloc(capacity , sizeof(int)) ;
		
	if(!tasks){
		free(q) ; 
		perror("Malloc error while init task buffer");
		return NULL ; 
	}
	
	if(pthread_mutex_init(&q->m , NULL) != 0){
		free(tasks) ; 
		free(q) ;
		perror("Mutex init error");
		return NULL ; 
	}
	if(pthread_cond_init(&q->cond_empty , NULL) != 0){
		free(tasks) ;
		pthread_mutex_destroy(&q->m) ; 
		free(q) ;
		perror("Mutex init error");
		return NULL ; 
	}
	if(pthread_cond_init(&q->cond_full , NULL) != 0){
		free(tasks) ;
		pthread_mutex_destroy(&q->m) ;
		pthread_cond_destroy(&q->cond_empty) ; 
		free(q) ;
		perror("Mutex init error");
		return NULL ; 
	}

	q->head = 0 ; 
	q->tail = 0  ;
	q->tasks = tasks ;
	q->length= 0 ;
	return q ; 
}

int destory_queue(task_queue_t *q){
	if(!q){
		perror("No  queue") ; 
		return -1 ; 
	}

	if(q->tasks){
		free(q->tasks) ; 
	}

	pthread_mutex_destroy(&q->m) ;
	pthread_cond_destroy(&q->cond_empty) ;
	pthread_cond_destroy(&q->cond_full) ; 

	free(q) ; 

	return 0;
}


int enqeue_task(task_queue_t *q, int task){
	if(!q){
		perror("No queue");
		return -1 ; 
	}
	
	pthread_mutex_lock(&q->m);
	while(q->length == q->capacity){
		// we cannot enqueue because its full, go to sleep
		pthread_cond_wait(&q->cond_empty , &q->m) ;  
	}

	q->tasks[q->tail] = task ;
	q->tail = (q->tail+1) % q->capacity ;
	q->length++ ;
	
	pthread_cond_signal(&q->cond_full) ; 
	
	pthread_mutex_unlock(&q->m);


	

	return 0 ;

}

int dequeue_task(task_queue_t *q){
	if(!q){
		perror("No queue");
		return -1 ; 
	}
	
	pthread_mutex_lock(&q->m) ; 
	while(q->length == 0){
		//we cannot deque go to sleep
		pthread_cond_wait(&q->cond_full , &q->m);
	}


	int task = q->tasks[q->head] ;
	q->head = (q->head + 1) % q->capacity ; 
	q->length-- ;
	
	pthread_cond_signal(&q->cond_empty) ; 

	pthread_mutex_unlock(&q->m) ; 

	return task ; 

}




