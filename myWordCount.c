/**
 * Lab #4
 * Last Update: October 23, 2018
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
 * Map Function     -
 * Reduce Function  -
 * Output Function  -
 * Generic Base     -
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
//-----------------------------What to be updated - Begin
#define MAX_STRING_LENGTH 255
#define MAX_WORDS 150
#define MAX_THREADS 4
//-----------------------------What to be updated - End

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
void threadArg_Initialize(struct threadArg tempStruct){
    for (int i = 0; i < (MAX_WORDS/MAX_THREADS); i++){
        tempStruct.tid = i;
        tempStruct.startIndex = floor(i*((MAX_WORDS/MAX_THREADS)));
        tempStruct.endIndex = floor((i+1)*((MAX_WORDS/MAX_THREADS)));
    }
}

char myBuff[MAX_WORDS][MAX_STRING_LENGTH];
struct myKeyVal wordList [MAX_WORDS];


// Function Definitions
void myInputFunc(void);
struct myKeyVal myMapFunc(char passString[]);
struct myKeyVal myReduceFunc(int startIn, int endIn);
void myOutputFunc(struct myKeyVal passStruct);

void *myMapThread_function(void* args);
int combineList_function(struct threadArg temp, int currIndex);



// Test File
int main(){
    
    // Initialize the thread arguments
    struct threadArg myThreads[MAX_THREADS];
    for (int i = 0; i< MAX_THREADS; i++){
        threadArg_Initialize(myThreads[i]);
    }

    // Read file and fill array with words
    myInputFunc();

    //array for threads
    pthread_t myThread_ID [MAX_THREADS]; 

    // Fill array of Key Value Pairs
    for (int i=0; i<MAX_THREADS; i++){
        //create thread and give function
        pthread_create(&myThread_ID[i], NULL, myMapThread_function, &myThreads[i]);
    }

    for (int i=0; i<MAX_THREADS; i++){
        //join thread
        pthread_join(myThread_ID[i], NULL);
    }

    int currentWordList_Index = 0;
    for (int i=0; i<MAX_THREADS; i++){
        combineList_function(myThreads[i], currentWordList_Index);
    }


    bool need_true = false;
    struct myKeyVal myTempStruct;
    int startIndex;
    for(int i=0; i < MAX_WORDS; i++ ){
        
        if (need_true == false) {
            startIndex=i;
        }
        if (i == MAX_WORDS -1 ){
            // Reduce the values
            myTempStruct = myReduceFunc(startIndex,i);
            //Print     void the structures
            myOutputFunc(myTempStruct);
        }
        else if ((strcmp(wordList[i].myKey, wordList[i+1].myKey)==0)){
            need_true = true;
        }
        else {
            need_true = false;
            // Reduce the values
            myTempStruct = myReduceFunc(startIndex,i);
            //Print the structures
            myOutputFunc(myTempStruct);
        }
    }

    return 0;
}

void myInputFunc(void){

    int i = 0;

    //Input words into array
    FILE *myInputFile;
    myInputFile = fopen( "list_words.txt", "r" );
    while(!feof(myInputFile)){
        fscanf(myInputFile, "%s", myBuff[i]);
        i++;
    }
   
    // Reffered from : https://www.quora.com/How-can-I-sort-a-2D-array-of-type-char-using-the-C++-sort-or-qsort-function
    // Sort the array 
    qsort(myBuff, MAX_WORDS, MAX_STRING_LENGTH, (int (*)(const void *, const void *))strcmp);
        
}


struct myKeyVal myMapFunc(char passString[]){
    struct myKeyVal tempStruct;
    for (int a=0; a<MAX_STRING_LENGTH; a++){
        tempStruct.myKey[a] = passString[a];
    }
    tempStruct.myValue = 1;
    return tempStruct;
}


struct myKeyVal myReduceFunc(int startIn, int endIn){
    struct myKeyVal tempStruct;
    int reducedValue = (endIn-startIn)+1;
    for (int a=0; a<MAX_STRING_LENGTH; a++){
        tempStruct.myKey[a] = wordList[startIn].myKey[a];
    }
    tempStruct.myValue = reducedValue;
    return tempStruct;
}



void myOutputFunc(struct myKeyVal passStruct){
    char spaceChar[1] = "";
    if ((strcmp(passStruct.myKey, spaceChar)==0)){
    }
    else{
        printf("%s , %d \n", passStruct.myKey, passStruct.myValue);
    }
}


void *myMapThread_function(void* args){
    struct threadArg *tempArg = (struct threadArg *) args;
    int j = 0;
    for(int i= tempArg->startIndex; i<tempArg->endIndex; i++){
        tempArg->solStruct[j] = myMapFunc(myBuff[i]);
        j++;
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
