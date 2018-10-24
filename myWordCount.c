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
//-----------------------------What to be updated - Begin
#define MAX_STRING_LENGTH 255
#define MAX_WORDS 1000
//-----------------------------What to be updated - End

// Global Variables
struct myKeyVal{
    char myKey[MAX_STRING_LENGTH];
    int myValue;
};
char myBuff[MAX_WORDS][MAX_STRING_LENGTH];
struct myKeyVal wordList [MAX_WORDS];


// Function Definitions
void myInputFunc(void);
struct myKeyVal myMapFunc(char passString[]);
struct myKeyVal myReduceFunc(int startIn, int endIn);
void myOutputFunc(struct myKeyVal passStruct);

// Test File
int main(){

    myInputFunc();

    // Fill array of Key Value Pairs
    for (int i=0; i<MAX_WORDS; i++){
        wordList[i]= myMapFunc(myBuff[i]);
    }
    
    bool need_true = false;
    struct myKeyVal myTempStruct;
    int startIndex;
    for(int i=0; i < MAX_WORDS; i++ ){
        
        if (need_true == false) {
            startIndex=i;
        }
        if (i == MAX_WORDS -1 ){
            //call the function and send Start index and i
            myTempStruct = myReduceFunc(startIndex,i);
            myOutputFunc(myTempStruct);
        }
        else if ((strcmp(wordList[i].myKey, wordList[i+1].myKey)==0)){
            need_true = true;
        }
        else {
            need_true = false;
            //call the function and send Start index and i
            myTempStruct = myReduceFunc(startIndex,i);
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
