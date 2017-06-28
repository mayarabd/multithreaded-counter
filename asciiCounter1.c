/****************************************************
 * name: Mayara Brandao Dusheyko
 * Description: This is a multi-threaded program to count the number of occurrences 
 *              of each ASCII characters in a text file provided by the user.
 *              It uses a one-dimentional array to store the counts. 
 ******************************************************/

#include <pthread.h>
#include <stdio.h>

#define ASCII_TABLE 128
#define NUM_THREADS 8

const int SIZE = 65336;
char buffer[SIZE];

//counts of ascii characters by the workers threads
int charCount[ASCII_TABLE];


//Struct for threads
struct ThreadData {
    char *threadStart;
    char *threadEnd;
    int *threadIndex;
};

//read file provided by user into buffer
int readFile(int argc, char *argv[]) {
    FILE *myFile;
    int charCounter = 0;
    
    if (argc != 2) {
        fprintf(stderr, "Error. Enter ./<program_name> <file_name.txt>\n");
        return (-1);
    }
    
    // Open file
    for( int i = 1; i < argc; i++) {
        myFile = fopen(argv[i], "r");
        if (myFile == NULL) {
            fprintf(stderr, "Can't open %s . Use format ./<program_name> <file_name.txt>\n", argv[i]);
            return (-1);
            
        }
        
        //read file into buffer
        int i = 0;
        
        while ((i < SIZE) && ((buffer[i] = fgetc(myFile)) != EOF)) {
            i++;
            charCounter++;
        }
        
        //close file
        fclose(myFile);
    }
    
    return charCounter;
}


//print result
void printResult() {
    
    printf("\n**** RESULT ****\nChar : Count\n");
    for (int i = 0; i < ASCII_TABLE; i++) {
        //if (charCount[i] > 0) {
        if (i > 32 && i != 127) {
            printf("   %c : %i \n", i, charCount[i]);
            //prints hexadecimal value
        } else {
            printf("0x%02x : %i \n", i, charCount[i]);
        }
       // }
    }
}

//counts the number of ascii characters in the array
void* asciiCounter(void *info) {
    
    struct ThreadData *td = (struct ThreadData*)info;
    char *start = (*td).threadStart;
    char *end = (*td).threadEnd;
    
    //checks every value in the array and increments its count
    //in the equivalent index position
    while (start != (end + 1)) {
        char value = *start;
        
        //address of array to increment count
        int* charAdd = (*td).threadIndex;
        
        //increment array at value position
        (*(charAdd + value))++;
        
        start++;
    }
    
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    
    //thread id
    pthread_t workers[NUM_THREADS];
    
    //thread attributes
    pthread_attr_t attr;
    
    //read file
    int fileSize = readFile(argc, argv);
    if (fileSize == -1) {
        return -1;
    }
    
    //get default attributes
    pthread_attr_init(&attr);
    
    //array with all the structs
    struct ThreadData tData[NUM_THREADS];
    //calculate partition for each thread
    int partSize = fileSize / NUM_THREADS;
    int remainder = fileSize % NUM_THREADS;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        int start, end;
        //sets start and end positions
        if (fileSize % NUM_THREADS == 0) {
            start = end = 0;
            start = i * partSize;
            end = start + (partSize - 1);
            
        } else {
            
            if (remainder > 0) {
                start = i * (partSize + 1);
                end = start + (partSize);
                remainder--;
                
            } else {
                start = end + 1;
                end = start + (partSize -1);
            }
        }
        
        //sets struct data
        //address of start and end reading position, and index to write counts
        tData[i].threadStart = (buffer + start);
        tData[i].threadEnd = (buffer + end);
        tData[i].threadIndex = &(charCount[0]);
        
        //create new thread
        pthread_create(&workers[i], &attr, asciiCounter, &tData[i]);
        
    }
    
    //wait for worker threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(workers[i], NULL);
    }
    
    //main thread prints the result
    printResult();
    
    return 0;
}