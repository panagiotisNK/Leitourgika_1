#include <stdio.h>                                              // Implementing the header files needed for printing,scanning,dynamically allocating,
#include <stdlib.h>                                             //threading, etc.
#include <pthread.h>

#define BARRIER __sync_synchronize()   
/*Setting "BARRIER" as a representasion of the "__sync_synchronize()" command/function throught the code.
This function sets a memory barrier,which practically means that it synchronises data in all threads.*/


struct func_args                                                //Creating a struct of three elements that the thread"s function is going to need in order to update the shared array.
{                                                               //The reason that the struct needs to be created is that the function linked to a thread can only take one argument.
	long int array_iteration;                                   
	long int array_size;
	long int *array;
};


void SA_update(struct func_args Arg);                           //Declaring the functions that are going to be created.

void *thread_func (void *routine_args);

void lock(long int iteration);

void unlock(long int iteration);


volatile long int *tokens;                                      //Declaring some global variables and arrays that are going to be needed outside of the main() function.
volatile long int *choice;

volatile long int resources;


int main()
{
    long int process_num;                                       //Declaring main() function's variables.
	long int i;
    
    resources = 0;
	
	printf("Insert the number of threads you wish to create:\n");    //Requiring number of threads/Shared array cells number from the user.
	printf("(Note:If you enter a non-integer number it is just going to be rounded.)\n");
    scanf("%ld",&process_num);	
    
	while(process_num<=0)                                            //Creating an exception in case the user inputs zero or negative value.
	{
    printf("\n!!ERROR!!\n");
    printf("Please insert a positive and non-zero value:\n");
    printf("(Note:If you enter a non-integer number it is just going to be rounded.)\n");
    scanf("%ld",&process_num);
    }
    
    tokens = malloc(sizeof(long int)*process_num);                   //Dynamically allocating the two global arrays with the size threat-count/Shared array size.
    choice = malloc(sizeof(long int)*process_num);
    
    system("cls");                                               // Clearing the terminal screen.
    
    printf("Creating Shared Array...\n");
    sleep(1);
    printf("Shared Array:\n");
    printf("{");

   
  struct func_args SharedArray ;                                 //Declaring the struct which will contain the Shared Array array size and iteration.
  
  SharedArray.array = malloc(process_num * sizeof(long int));    //Dynamically allocating the Shared Array with the size of the thread count.
  SharedArray.array_size = process_num;                          //Setting the array size as the thread count.
  
    
    for (i=0;i<process_num;i++)                                  //Filling Shared Array's cells with zeroes.
    {
    SharedArray.array[i]=0;                                      
    }    
    

    for (i=0;i<process_num;i++)                                  //Printing the Shared Array with all zeroes.
   {
 	printf("%ld,",SharedArray.array[i]);
   }
    
	printf("\b");
    printf("}\n");
    printf("\n\nPress Enter to Continue...\n");                  //Requesting from the user a key-press to continue.
    
    getch();                                                     //Getting the input and continuing.
	
    system("cls");
    printf("Now updating the cells of the Shared Array...\n");
   
    sleep(2);                                                    //Giving user some time to read all the messages.
   


	pthread_t thread_var[process_num];                           //Declaring a thread-type variable.
	
	for (i=0;i<process_num;i++)                                  //Starting the multi-thread process.The code inside the for-loop will be executed "process_num" times, 
	{                                                            //which the user has inputed from the previous lines.
	
	    SharedArray.array_iteration=i;                           //Giving the array_iteration element the value of the actual thread number for each iteration. 
		pthread_create(&thread_var[i] ,NULL,&thread_func,&SharedArray);     //Creating a thread that is linked to thread_func() function while the struct argument (SharedArray) is passed.
		
		pthread_join(thread_var[i],NULL);                                   //Executing the pthread_join() function, similar to waitpid() for processes, which for each threads execution
	}	                                                                    //before terminating the program.

	

	
free(SharedArray.array);                                          //Freeing the allocated memory blocks from the shared array using the free() function.

return 0;                                                         //Returning 0 as an exit value of the main function.
}



void *thread_func (void *routine_args)                            //Writing the code that the thread will actually execute using the thread_func function.
{
struct func_args *actual_args= routine_args;                      //First, implementing the arguments passed in a struct in order to properly use them.

lock(actual_args->array_iteration);                               //Secondly, calling the lock() function for the current thread/iteration,which actually is the first part of Lamport's bakery algorithm.

SA_update(*actual_args);                                          //Thridly, calling the helping function that will update the shared array,by implementing again the whole struct as an argument.
	
unlock(actual_args->array_iteration);                             //Lastly, calling the unlock() function for the current thread/iteration,which actually is the last part of Lamport's bakery algorithm.
	
}




void lock(long int iteration)                                     //Writing lock function's code:
{
    
	choice[iteration] = 1;                                        //Setting choice variable of current thread to be 1 which is the logical value "TRUE",so you actually enable choice. 
	BARRIER;
    long int max_token = 0;                                       
    long int i;
    long int other;
    
	for ( i = 0; i < iteration+1; i++)                            //This for-loop will scan all the previous token values by accesing the tokens[] array to find the lagrest token value given until now.
	{
        long int token = tokens[i];
        max_token = token > max_token ;                           //Here,when the maximum token is found it gets placed into the array in the current position.Then,
    }
    tokens[iteration] = max_token + 1;                            //it gets increased by one,cause it is given to the new thread and not any of the previous ones.
    BARRIER;                                                      //Take into consideration that each thread must have its unique token,that is actually why that algorithm was created.Only if two threads are inserted at the exact same time will be able to have the same token number.
    choice[iteration] = 0;                                        
    BARRIER;
    for (other = 0;other < iteration+1; other++)                  //This for-loop acceses the choice and tokens array for the current thread to check if the bakery algorithm conditions
	{                                                             //are preserved.
        while (choice[other]) { }
        BARRIER;
        while (tokens[other] != 0 && (tokens[other] < tokens[iteration] ||(tokens[other] == tokens[iteration] && other < iteration))) { }
    }
}


void unlock(long int iteration)                                   //This function sets the the current token's value back to 0 as the currents thread has completed its execution code. 
{
	BARRIER;
    tokens[iteration] = 0;
}


void SA_update(struct func_args Arg)                             //Finally, that is the helping function that actually updates the Shared Array in the way that"s been asked.
{
	/*In the specicfic programm this if-statement is actually not necessary.That is because we have implemented memory barriers which syncronise data a.k.a. rersources among  different threads.
	However you can try removing the barriers and it will work correctly.*/
 if (resources != 0)                                            //When resources value isn't 0 it means that another thread is using them(check 1 line after the if-statement.
	    {
        printf("Resources were requested by thread number %d, but they are still occupied by thread number %d!\n",Arg.array_iteration, resources);    //Printing the number of the thread that is still using recources
        }
    
	resources = Arg.array_iteration;                            //Setting the resourses value to the number of the current thread. 
	
	long int i;
	
	for (i=0; i<Arg.array_size ;i++)                            //This for-loop updates all the cells of the Shared Array.
	{
	Arg.array[i] = Arg.array[i] + Arg.array_iteration;          //It fills every cell with the value that is calculated by this formula: Shared Array[i]=Shared Array[i] + current array's iteration .
    }
    
    printf("Shared Array after i=%ld:\n",Arg.array_iteration);
    printf("{");
    
	for (i=0; i<Arg.array_size ;i++)                            //Print the shared array after every update.
    {
    	printf("%ld,",Arg.array[i]);
	}
	printf("\b");
	printf("}\n");
	
	BARRIER;
	
	sleep(1);
	
	resources = 0;                                             //Setting resources back to 0, so that the next thread in line can start using the resources.
}
