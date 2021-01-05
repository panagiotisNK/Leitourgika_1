#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define BARRIER __sync_synchronize()


struct func_args
{
	long int array_iteration;
	long int array_size;
	long int *array;
};


void SA_update(struct func_args Arg);

void *thread_func (void *routine_args);

void lock(long int iteration);

void unlock(long int iteration);


volatile long int *tokens;
volatile long int *choice;

volatile long int resources;


int main()
{
    long int process_num;
	long int i;
    
    resources = 0;
	
	printf("Insert the number of threads you wish to create:\n");
    scanf("%ld",&process_num);	
    
	while(process_num<=0)
	{
    printf("Please insert a positive and non-zero value:\n");
    scanf("%ld",&process_num);
    }
    
    tokens = malloc(sizeof(long int)*process_num);
    choice = malloc(sizeof(long int)*process_num);
    
    system("cls");
    
    printf("Creating Shared Array...\n");
    sleep(1);
    printf("Shared Array:\n");
    printf("{");

   
  struct func_args SharedArray ;
  
  SharedArray.array = malloc(process_num * sizeof(long int));
  SharedArray.array_size = process_num;
  
    
    for (i=0;i<process_num;i++)
    {
    SharedArray.array[i]=0;
    }    
    

    for (i=0;i<process_num;i++)
   {
 	printf("%ld,",SharedArray.array[i]);
   }
    
	printf("\b");
    printf("}\n");
    printf("\n\nPress Enter to Continue...\n");
    
    getch();
	
    system("cls");
    printf("Now updating the cells of the Shared Array...\n");
   
    sleep(2);
   


	pthread_t thread_var[process_num];
	
	for (i=0;i<process_num;i++)
	{   
	    SharedArray.array_iteration=i;
		pthread_create(&thread_var[i] ,NULL,&thread_func,&SharedArray);
		
		pthread_join(thread_var[i],NULL);
	}
	

	
free(SharedArray.array);

return 0;
}



void *thread_func (void *routine_args)
{
struct func_args *actual_args= routine_args;

lock(actual_args->array_iteration);

SA_update(*actual_args);
	
unlock(actual_args->array_iteration);
	
}




void lock(long int iteration)
{
    
	choice[iteration] = 1;
	BARRIER;
    long int max_token = 0;
    long int i;
    long int other;
    
	for ( i = 0; i < iteration+1; i++)
	{
        long int token = tokens[i];
        max_token = token > max_token ;
    }
    tokens[iteration] = max_token + 1;
    BARRIER;
    choice[iteration] = 0;
    BARRIER;
    for (other = 0;other < iteration+1; other++) 
	{
        while (choice[other]) { }
        BARRIER;
        while (tokens[other] != 0 && (tokens[other] < tokens[iteration] ||(tokens[other] == tokens[iteration] && other < iteration))) { }
    }
}


void unlock(long int iteration)
{
	BARRIER;
    tokens[iteration] = 0;
}


void SA_update(struct func_args Arg)
{
 if (resources != 0)
	    {
        printf("Resources were requested by thread number %d, but they are still occupied by thread number %d!\n",Arg.array_iteration, resources);
        }
    
	resources = Arg.array_iteration;
	
	long int i;
	
	for (i=0; i<Arg.array_size ;i++)
	{
	Arg.array[i] = Arg.array[i] + Arg.array_iteration;
    }
    
    printf("Shared Array after i=%ld:\n",Arg.array_iteration);
    printf("{");
    
	for (i=0; i<Arg.array_size ;i++)
    {
    	printf("%ld,",Arg.array[i]);
	}
	printf("\b");
	printf("}\n");
	
	BARRIER;
	
	sleep(1);
	
	resources = 0;
}
