#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

sem_t sq;
sem_t sp;

void *p(void *arg){
	//E1: print the date, set an new date
	printf("E1:\n");
	system("date");
	
	//E5: show the calendar
	printf("E5:\n");
	system("calendar");
	
	//E8: show a list of files and directories
	printf("E8:\n");
	system("ls -l");
	
	sem_post(&sq);
	sem_wait(&sp);
	
	//E9:
	printf("E9:\n");
	system("");
	
	//E7:
	printf("E7:\n");
	system("");
	
	sem_post(&sq);
}

void *q(void *arg){
	
	sem_wait(&sq);
	
	//E2: show the list of processes
	printf("E2:\n");
	system("ps -l");
	
	//E3: 
	printf("E3:\n");
	system("");
	
	//E6:
	printf("E6:\n");
	system("");
	
	sem_post(&sp);
	sem_wait(&sq);
	
	//E4:
	printf("E4:\n");
	system("");
}


int main(int argc, char* arvg[]){
	
	//Initializing Semaphores
	sem_init(&sq, 0, 0);
	sem_init(&sp, 0, 0);
	
	pthread_t p_thread;
	if(pthread_create(&p_thread, NULL, &p, NULL) != 0){
		perror("Failed");
		return 1;
	}
	
	pthread_t q_thread;
	if(pthread_create(&q_thread, NULL, &q, NULL) != 0){
		perror("Failed");
		return 1;
	}
	
	pthread_join(p_thread, NULL);
	pthread_join(q_thread, NULL);
	
	sem_destroy(&sq);
	sem_destroy(&sp);
	
}
