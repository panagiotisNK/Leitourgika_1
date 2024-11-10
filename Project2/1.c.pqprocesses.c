#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>

sem_t sq;
sem_t sp;

void *p(void *arg){
	//E1: print the date
	printf("E1:\n");
	system("date");
	
	//E5: show the calendar
	printf("\nE5:\n");
	system("cal");
	
	//E8: show a list of files and directories
	printf("\nE8:\n");
	system("ls -l");
	
	sem_post(&sq);
	sem_wait(&sp);
	
	//E9: show how many hours the current pc is on
	printf("\nE9:\n");
	system("uptime");
	
	//E7: show user's info (username)
	printf("\nE7:\n");
	system("whoami");
	
	sem_post(&sq);
}

void *q(void *arg){
	
	sem_wait(&sq);
	
	//E2: show the list of processes
	printf("\nE2:\n");
	system("ps -l");
	
	//E3: show RAM
	printf("\nE3:\n");
	system("free -h");
	
	//E6:
	printf("\nE6:\n");
	system("echo 'hello'");
	
	sem_post(&sp);
	sem_wait(&sq);
	
	//E4: show kernel info
	printf("\nE4:\n");
	system("uname -a");
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
