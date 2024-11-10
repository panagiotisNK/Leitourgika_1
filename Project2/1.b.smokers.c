#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

// A seller semaphore represents that the seller left two items on the table
sem_t sellerSem;

// Each smoker semaphore represents when a smoker has the items they need
sem_t smokerSem[3];

// This is an array of strings describing what each type of smoker needs
char* smokerType[3] = { "matches & tobacco","matches & paper","tobacco & paper"};

// This list represents item types that are on the table. This should corrispond with the smokerTypes, such that each item is the one the smoker has. So the first item would be paper, then tobacco, then matches.
bool itemsOnTable[3] = { false, false, false };

// Each pusher pushes a certian type item, manage these with this semaphore
sem_t pushSem[3];

//Smoker function, handles waiting for the item’s that they need, and then smoking. Repeat this three times

void* smoker(void* arg){
	
	int sId = *(int*) arg;
	int typeId = sId % 3;

	// Smoke 3 times
	int i;
	for(i = 0; i < 3; ++i){
		printf("Smoker %d Waiting for %s\n",
		sId, smokerType[typeId]);

		// Wait for the proper combination of items to be on the table
		sem_wait(&smokerSem[typeId]);

		// Make the cigarette and smoke before releasing the seller semaphore
		printf("Smoker %d Now making a cigarette\n", sId);
		printf("Smoker %d Now smoking\n", sId);
		sleep(1);
		sem_post(&sellerSem);
		}

	return NULL;
	
}


// This semaphore gives the pusher exclusive access to the items on the table
sem_t pusherLock;


//The pusher is responsible for releasing the proper smoker semaphore when the right item’s are on the table.

void* pusher(void* arg){
	
	int pushId = *(int*) arg;
	
	int i;
	for(i = 0; i < 12; ++i){
		// Wait for this pusher to be needed
		sem_wait(&pushSem[pushId]);	
		sem_wait(&pusherLock);

		// Check if the other item we need is on the table
		if(itemsOnTable[(pushId + 1) % 3]){
			itemsOnTable[(pushId + 1) % 3] = false;
			sem_post(&smokerSem[(pushId + 2) % 3]);
		}
		else if(itemsOnTable[(pushId + 2) % 3]){
			itemsOnTable[(pushId + 2) % 3] = false;
			sem_post(&smokerSem[(pushId + 1) % 3]);
		}
		else{
			// The other item’s aren’t on the table yet
			itemsOnTable[pushId] = true;
		}

		sem_post(&pusherLock);
		
	}

	return NULL;
	
}


//The seller puts two items on the table

void* seller(void* arg){
	
	int sellId = *(int*) arg;

	int i;
	for(i = 0; i < 3; ++i){
		sleep(1);

		// Wait for a lock on the seller
		sem_wait(&sellerSem);

		// Release the items this seller gives out
		sem_post(&pushSem[sellId]);
		sem_post(&pushSem[(sellId + 1) % 3]);

		// Say what type of items we just put on the table
		printf("Seller giving out %s\n", smokerType[(sellId + 2) % 3]);
	}

	return NULL;
	
}


//The main thread handles the agent’s arbitration of items.

int main(int argc, char* arvg[]){
	
	// Seed our random number since we will be using random numbers
	srand(time(NULL));

	// There is only one seller semaphore since only one set of items may be on the table at any given time. A values of 1 = nothing on the table
	sem_init(&sellerSem, 0, 1);

	// Initalize the pusher lock semaphore
	sem_init(&pusherLock, 0, 1);

	// Initialize the semaphores for the smokers and pusher
	
	int i;
	for(i = 0; i < 3; ++i){
		sem_init(&smokerSem[i], 0, 0);
		sem_init(&pushSem[i], 0, 0);
	}

	// Smoker ID’s will be passed to the threads. Allocate the ID’s on the stack
	int sId[3];

	pthread_t smokerThread[3];

	// Create the 6 smoker threads with IDs
	for(i = 0; i < 3; ++i){
		sId[i] = i;

		if(pthread_create(&smokerThread[i], NULL, smoker, &sId[i]) == EAGAIN){
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	// Pusher ID’s will be passed to the threads. Allocate the ID’s on the stack
	int pushId[3];

	pthread_t pusherThread[3];

	for(i = 0; i < 3; ++i){
		pushId[i] = i;

		if(pthread_create(&pusherThread[i], NULL, pusher, &pushId[i]) == EAGAIN){
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	// Seller's ID will be passed to the threads. Allocate the IDs on the stack
	int sellId[3];

	pthread_t sellerThread[3];

	for(i = 0; i < 3; ++i){
		sellId[i] =i;

		if(pthread_create(&sellerThread[i], NULL, seller, &sellId[i]) == EAGAIN){
			perror("Insufficient resources to create thread");
			return 0;
		}
	}

	// Make sure all the smokers are done smoking
	for(i = 0; i < 3; ++i){
		pthread_join(smokerThread[i], NULL);
	}

	return 0;
	
}
