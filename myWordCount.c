/**
 * Lab #4
 * Last Update: October 24, 2018
 * 
 * Developers:
 * 1) Dean Fernandes
 *      301274908
 *      deanf@sfu.ca
 * 
 * 2) Sheel Soneji
 *      301295318
 *      ssoneji@sfu.ca
 * 
 * Tasks:
 * 
 * Input Function   - Done
 * Map Function     - Done
 * Reduce Function  - Done
 * Output Function  - Done
 * Generic Base     - Done
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <malloc.h>
#include <omp.h>
#include <math.h>
//-----------------------------What to be updated - Begin------------------------------------for tester
#define MAX_STRING_LENGTH 30
#define MAX_WORDS 7500
#define MAX_THREADS 4
//-----------------------------What to be updated - End------------------------------------for tester

// Global Variables
struct myKeyVal{
    char myKey[MAX_STRING_LENGTH];
    int myValue;
};
struct threadArg{
    int tid;
    int startIndex;
    int endIndex;
    struct myKeyVal solStruct[MAX_WORDS/MAX_THREADS];
};

struct threadArg_reduce{
    int tid;
    int startIndex;
    int endIndex;
};
struct similarWords{
    struct myKeyVal solStruct[MAX_WORDS];
    int count;
    struct myKeyVal finalStruct;
};

char myBuff[MAX_WORDS][MAX_STRING_LENGTH];
struct myKeyVal wordList [MAX_WORDS];
struct similarWords similarWords_Array [MAX_WORDS];
struct myKeyVal finalList [MAX_WORDS];



// Function Definitions
void myInputFunc(void);
struct myKeyVal myMapFunc(char passString[]);
struct myKeyVal myReduceFunc(int startIn, int endIn);
void myOutputFunc(struct myKeyVal passStruct);
void *myMapThread_function(void* args);
int combineList_function(struct threadArg temp, int currIndex);
struct similarWords similarWords_Initialize(int startIndex, int endIndex);
void *myReduceThread_function(void* args);



// Test File
int main(){
    
    // Initialize the thread arguments
    struct threadArg myThreads[MAX_THREADS];

    for (int i = 0; i< MAX_THREADS; i++){
        myThreads[i].tid = i;
        myThreads[i].startIndex = floor(i*((MAX_WORDS/MAX_THREADS)));
        myThreads[i].endIndex = floor((i+1)*((MAX_WORDS/MAX_THREADS)));

    }

    // Calling Input Function
    myInputFunc();
    /*
    //array for threads
    pthread_t myThread_ID [MAX_THREADS]; 

    //create thread and give map function
    for (int i=0; i<MAX_THREADS; i++){
        pthread_create(&myThread_ID[i], NULL, myMapThread_function, (void *) &myThreads[i]);
    }

    for (int i=0; i<MAX_THREADS; i++){
        //join thread
        pthread_join(myThread_ID[i], NULL);
    }
    */

    //Calling Map Function (Multi Threading)
    #pragma omp parallel num_threads (MAX_THREADS)
    {
        int tid = omp_get_thread_num();
        myMapThread_function((void*) &myThreads[tid]);
    }

    int currentWordList_Index = 0;
    for (int i=0; i<MAX_THREADS; i++){
        currentWordList_Index = combineList_function(myThreads[i], currentWordList_Index);
    }


    bool need_true = false;
    //struct myKeyVal myTempStruct;
    int startIndex;
    int index_similarWords_Array = 0;
    for(int i=0; i < MAX_WORDS; i++ ){
        
        if (need_true == false) {
            startIndex=i;
        }
        // If we go through the entire loop and not find the matching key
        if (i == MAX_WORDS -1 ){
            similarWords_Array[index_similarWords_Array]=similarWords_Initialize(startIndex, i);
            index_similarWords_Array++;
        }
        // When we kind the same key value - begin
        else if ((strcmp(wordList[i].myKey, wordList[i+1].myKey)==0)){
            need_true = true;
        }
        // When we kind the same key value - end
        else {
            need_true = false;
            similarWords_Array[index_similarWords_Array]=similarWords_Initialize(startIndex, i);
            index_similarWords_Array++;
        }
    }
    //array for threads
    //pthread_t myThread_ID_Reduce [index_similarWords_Array];
    
    //-----------------------------------------------------------------------
    //pthread_t myThread_ID_Reduce [MAX_THREADS];
    struct threadArg_reduce myThreads2[MAX_THREADS];
    
    for (int i = 0; i< MAX_THREADS; i++){
        myThreads2[i].tid = i;
        myThreads2[i].startIndex = floor(i*((index_similarWords_Array/MAX_THREADS)));
        myThreads2[i].endIndex = floor((i+1)*((index_similarWords_Array/MAX_THREADS)));
    }
    //-----------------------------------------------------------------------    
    //create thread and give reduce function
    // for(int j=0; j<MAX_THREADS; j++){
    //     pthread_create(&myThread_ID_Reduce[j], NULL, myReduceThread_function, (void *) &myThreads2[j]);        
    // }

    // for (int i=0; i<MAX_THREADS; i++){
    //     //join thread
    //     pthread_join(myThread_ID_Reduce[i], NULL);
    // }

    //Calling Reduce Function (Multi Threading)
    #pragma omp parallel num_threads (MAX_THREADS)
    {
        int tid2 = omp_get_thread_num();
        myReduceThread_function((void*) &myThreads2[tid2]);
    }

    // for (int k=0; k<MAX_WORDS; k++){
    //     myOutputFunc(similarWords_Array[k].finalStruct);
    // }


    //Calling Output Function
    int maxVal = similarWords_Array[0].finalStruct.myValue;
    for(int j = 0; j < MAX_WORDS ; j++){
        int maxIndex = 0;
        maxVal = similarWords_Array[j].finalStruct.myValue;
        for(int i = 0; i < MAX_WORDS ; i++){
            if(maxVal <= similarWords_Array[i].finalStruct.myValue){
                maxVal = similarWords_Array[i].finalStruct.myValue;
                maxIndex = i;
            }
        }
        myOutputFunc(similarWords_Array[maxIndex].finalStruct);
        similarWords_Array[maxIndex].finalStruct.myValue = -1;
    }

    return 0;
}

// input function
void myInputFunc(void){
    int i = 0;
    //Input words into array
    FILE *myInputFile;
//-----------------------------What to be updated - Begin------------------------------------for tester
    myInputFile = fopen( "list_words.txt", "r" );
//-----------------------------What to be updated - End------------------------------------for tester
    while(!feof(myInputFile)){
        fscanf(myInputFile, "%s", myBuff[i]);
        i++;
    }
    // Reffered from : https://www.quora.com/How-can-I-sort-a-2D-array-of-type-char-using-the-C++-sort-or-qsort-function
    // Sort the array 
    qsort(myBuff, MAX_WORDS, MAX_STRING_LENGTH, (int (*)(const void *, const void *))strcmp);
        
}

//map function
struct myKeyVal myMapFunc(char passString[]){
    struct myKeyVal tempStruct;
    for (int a=0; a<MAX_STRING_LENGTH; a++){
        tempStruct.myKey[a] = passString[a];
    }
    tempStruct.myValue = 1;
    return tempStruct;
}

// reduce  function
struct myKeyVal myReduceFunc(int startIn, int endIn){
    struct myKeyVal tempStruct;
    int reducedValue = (endIn-startIn)+1;
    for (int a=0; a<MAX_STRING_LENGTH; a++){
        tempStruct.myKey[a] = wordList[startIn].myKey[a];
    }
    tempStruct.myValue = reducedValue;
    return tempStruct;
}


// output function
void myOutputFunc(struct myKeyVal passStruct){
    char spaceChar[1] = "";
    if ((strcmp(passStruct.myKey, spaceChar)==0)){
    }
    else{
        printf("%s , %d \n", passStruct.myKey, passStruct.myValue);
    }
}

// thread function for map input
void *myMapThread_function(void* args){
    struct threadArg *tempArg = (struct threadArg *) args;
    int j = 0;
    for(int i= tempArg->startIndex; i<tempArg->endIndex; i++){
        tempArg->solStruct[j] = myMapFunc(myBuff[i]);
        j++;
    }
    return NULL;
}

// thread function for reduce input
void *myReduceThread_function(void* args){
    struct threadArg_reduce *tempArg = (struct threadArg_reduce *) args;
    for(int i= tempArg->startIndex; i< tempArg->endIndex; i++){
        for (int a=0; a<MAX_STRING_LENGTH; a++){
            similarWords_Array[i].finalStruct.myKey[a] = similarWords_Array[i].solStruct[0].myKey[a];
        }
        similarWords_Array[i].finalStruct.myValue = similarWords_Array[i].count;
    }
    return NULL;
} 


//return back current combineList index
int combineList_function(struct threadArg temp, int currIndex){
    for(int i=0; i<(MAX_WORDS/MAX_THREADS); i++){
        for(int j=0; j<(MAX_STRING_LENGTH); j++){
            wordList[currIndex].myKey[j] = temp.solStruct[i].myKey[j];
        }
        wordList[currIndex].myValue = temp.solStruct[i].myValue;
        currIndex++;
    }
    return currIndex;
}

// return back similarWords struct
struct similarWords similarWords_Initialize(int startIndex, int endIndex){
    struct similarWords tempStruct;
    int maxCount = (endIndex - startIndex)+1;
    tempStruct.count = maxCount;
    for(int i=0; i<maxCount; i++){
        for(int j=0; j<MAX_STRING_LENGTH; j++){
           tempStruct.solStruct[i].myKey[j] = wordList[startIndex].myKey[j];
        }
        tempStruct.solStruct[i].myValue = wordList[startIndex].myValue;
    }
    return tempStruct;
}
