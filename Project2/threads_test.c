#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define MEMBAR __sync_synchronize()


struct func_args
{
	long int array[1000];
	long int array_iteration;
};


void *SA_update (void *routine_args);



//volatile int tickets[THREAD_COUNT];
//volatile int choosing[THREAD_COUNT];

//volatile int resource;


int main()
{
    long int process_num;
	long int i;
	long int k;
	long int j;
	long int t;
	

	
	
	printf("Poses diergasies theleis na ftiaxtoyn(akeraios arithmos)\n");
    scanf("%ld",&process_num);	
    system("cls");
   
   //struct func_args SharedArray =malloc(sizeof(struct func_args) + process_num);
  struct func_args SharedArray ;
  
    
    for (i=0;i<process_num;i++)
    {
    SharedArray.array[i]=0;
    }    
    
   
 
 /*for (k=0;k<process_num;k++)
 {
 	printf("%ld\n",SharedArray.array[k]);
 }
*/

/*for(k=0;k<process_num;k++)
   {
    SharedArray.array_iteration=k;
    SA_update(&SharedArray);
   }
*/


	pthread_t thread_var[process_num];
	
	for (t=0;t<process_num;t++)
	{   
	    SharedArray.array_iteration=t;
		pthread_create(&thread_var[t],NULL,&SA_update,&SharedArray);
	}
	
	for (t=0;t<process_num;t++)
	{
		pthread_join(&thread_var[t],NULL);
	}



/*for (j=0;j<process_num;j++)
 {
 	printf("%ld\n",SharedArray.array[j]);
 }
*/



}



void *SA_update (void *routine_args)
{
struct func_args *actual_args= routine_args;
	
	actual_args->array[actual_args->array_iteration]= actual_args->array[actual_args->array_iteration]+actual_args->array_iteration;
	
	//printf("%ld\n",actual_args->array[actual_args->array_iteration]);
	
	
	printf("%ld\n",actual_args->array_iteration);
	
	free(actual_args);
}


