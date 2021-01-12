#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

// An agent semaphore represents items on the table
sem_t agent_ready;

// Each smoker semaphore represents when a smoker has the items they need
sem_t smoker_semaphors[3];

// This is an array of strings describing what each smoker type needs
char* smoker_types[3] = { "matches & tobacco","matches & paper","tobacco & paper"};

// This list represents item types that are on the table. This should corrispond
// with the smoker_types, such that each item is the one the smoker has. So the
// first item would be paper, then tobacco, then matches.
bool items_on_table[3] = { false, false, false };

// Each pusher pushes a certian type item, manage these with this semaphore
sem_t pusher_semaphores[3];

/**
* Smoker function, handles waiting for the item’s that they need, and then
* smoking. Repeat this three times
*/
void* smoker(void* arg)
{
int smoker_id = *(int*) arg;
int type_id = smoker_id % 3;

// Smoke 3 times
for (int i = 0; i < 3; ++i)
{
printf("Smoker %d Waiting for %s\n",
smoker_id, smoker_types[type_id]);

// Wait for the proper combination of items to be on the table
sem_wait(&smoker_semaphors[type_id]);

// Make the cigarette before releasing the agent
//printf("Smoker %d Now making a cigarette\n", smoker_id);
//sleep(1);
//sem_post(&agent_ready);

// We’re smoking now
printf("Smoker %d Now smoking\n", smoker_id);
sleep(1);
sem_post(&agent_ready);
}

return NULL;
}

// This semaphore gives the pusher exclusive access to the items on the table
sem_t pusher_lock;

/**
* The pusher is responsible for releasing the proper smoker semaphore when the
* right item’s are on the table.
*/
void* pusher(void* arg)
{
int pusher_id = *(int*) arg;

for (int i = 0; i < 12; ++i)
{
// Wait for this pusher to be needed
sem_wait(&pusher_semaphores[pusher_id]);
sem_wait(&pusher_lock);

// Check if the other item we need is on the table
if (items_on_table[(pusher_id + 1) % 3])
{
items_on_table[(pusher_id + 1) % 3] = false;
sem_post(&smoker_semaphors[(pusher_id + 2) % 3]);
}
else if (items_on_table[(pusher_id + 2) % 3])
{
items_on_table[(pusher_id + 2) % 3] = false;
sem_post(&smoker_semaphors[(pusher_id + 1) % 3]);
}
else
{
// The other item’s aren’t on the table yet
items_on_table[pusher_id] = true;
}

sem_post(&pusher_lock);
}

return NULL;
}

/**
* The agent puts items on the table
*/
void* agent(void* arg)
{
int agent_id = *(int*) arg;

for (int i = 0; i < 3; ++i)
{
sleep(1);

// Wait for a lock on the agent
sem_wait(&agent_ready);

// Release the items this agent gives out
sem_post(&pusher_semaphores[agent_id]);
sem_post(&pusher_semaphores[(agent_id + 1) % 3]);

// Say what type of items we just put on the table
printf("Agent  giving out %s\n",smoker_types[(agent_id + 2) % 3]);
}

return NULL;
}

/**
* The main thread handles the agent’s arbitration of items.
*/
int main(int argc, char* arvg[])
{
// Seed our random number since we will be using random numbers
srand(time(NULL));

// There is only one agent semaphore since only one set of items may be on
// the table at any given time. A values of 1 = nothing on the table
sem_init(&agent_ready, 0, 1);

// Initalize the pusher lock semaphore
sem_init(&pusher_lock, 0, 1);

// Initialize the semaphores for the smokers and pusher
for (int i = 0; i < 3; ++i)
{
sem_init(&smoker_semaphors[i], 0, 0);
sem_init(&pusher_semaphores[i], 0, 0);
}

// Smoker ID’s will be passed to the threads. Allocate the ID’s on the stack
int smoker_id[3];

pthread_t smoker_thread[3];

// Create the 6 smoker threads with IDs
for (int i = 0; i < 3; ++i)
{
smoker_id[i] = i;

if (pthread_create(&smoker_thread[i], NULL, smoker, &smoker_id[i]) == EAGAIN)
{
perror("Insufficient resources to create thread");
return 0;
}
}

// Pusher ID’s will be passed to the threads. Allocate the ID’s on the stack
int pusher_id[3];

pthread_t pusher_thread[3];

for (int i = 0; i < 3; ++i)
{
pusher_id[i] = i;

if (pthread_create(&pusher_thread[i], NULL, pusher, &pusher_id[i]) == EAGAIN)
{
perror("Insufficient resources to create thread");
return 0;
}
}

// Agent ID’s will be passed to the threads. Allocate the ID’s on the stack
int agent_id[3];

pthread_t agent_thread[3];

for (int i = 0; i < 3; ++i)
{
agent_id[i] =i;

if (pthread_create(&agent_thread[i], NULL, agent, &agent_id[i]) == EAGAIN)
{
perror("Insufficient resources to create thread");
return 0;
}
}

// Make sure all the smokers are done smoking
for (int i = 0; i < 3; ++i)
{
pthread_join(smoker_thread[i], NULL);
}

return 0;
}
