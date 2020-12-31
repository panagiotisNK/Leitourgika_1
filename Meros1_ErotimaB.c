#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

#define MEMBAR __sync_synchronize()


struct func_args
{
	long int array[1000];
	long int array_iteration;
};


void *SA_update (struct func_args* A);



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
	
	
	
	
	
	
/*	pthread_t thread_var[process_num];
	
	for (t=0;t<process_num;t++)
	{
		pthread_create(&thread_var[t],NULL,&SA_update)
	}
*/	
	
	printf("Poses diergasies theleis na ftiaxtoyn(akeraios arithmos)\n");
    scanf("%ld",&process_num);	
    system("cls");
    printf("Shared Array: \n{\n");
   
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

for(k=0;k<process_num;k++)
   {
    SharedArray.array_iteration=k;
    SA_update(&SharedArray);
   }

/*for (j=0;j<process_num;j++)
 {
 	printf("%ld\n",SharedArray.array[j]);
 }
*/

printf("}");

}



void *SA_update (struct func_args* A)
{
//	long int i;
//	i= A.array_iteration;
	
	A->array[A->array_iteration]= A->array[A->array_iteration]+A->array_iteration;
	
	printf("%ld\n",A->array[A->array_iteration]);
	//printf("%ld\n",A->array_iteration);
}






