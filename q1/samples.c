#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <time.h>

int main(int argc, char* argv[]){

    if(argc != 4){
        printf("Error on usage: ./samples filename numPrints numChar.\n");
        exit(EXIT_FAILURE);
    }

    FILE *filepointer;

    int numPrints = atoi(argv[2]); // convert string input to int
    int numChar = atoi(argv[3]); // convert string input to int
    filepointer = fopen(argv[1], "r");

    if(filepointer == NULL){
        printf("This file cannot be opened.\n");
        exit(EXIT_FAILURE);
    }

    fseek(filepointer, 0L, SEEK_END);
    long int file_size = ftell(filepointer);
    rewind(filepointer);

    // ***** error messages *****

    if (numPrints <= 0 || numChar <= 0 ){
        printf("numPrints or numChars are not a positive number. Please insert a valid input.\n");
        exit(EXIT_FAILURE);
    }

    if(numPrints > file_size){
        printf("There are not enough characters in this file to print that many fragments\n");
        exit(EXIT_FAILURE);
    }

    if(numPrints > (file_size - numChar + 1)){
        printf("Not enough file positions can be generated to print that many different fragments\n");
        exit(EXIT_FAILURE);
    }   

    // **********

    int randoNum = 0, flag=0;
    int randoNumArr[file_size]; // array to store the random numbers so we don't repeat them
    memset(randoNumArr, -1, file_size*sizeof(int)); // initialize to 0

    srand(time(0)); // different sequence of random numbers on every program run
 
    for(int i = 0; i < numPrints; i++){

        flag=0;

        randoNum = rand() % (file_size - numChar + 1) ; // random numbers from 0 to (file_size - numChar + 1) ex: file has size 4 and numChar is 1, rando num = rand % 4 -> goes from 0 to 3

        // ***** check if number already printed *****
        for (int k = 0; k < file_size; k++){
            if (randoNumArr[k] == randoNum){
                flag = 1;
                break;
            }
        }

        if (flag == 1){
            i--;
            continue;
        } else {
            randoNumArr[i] = randoNum;
        } 
        // **********

        fseek (filepointer, randoNum, SEEK_SET); // seek file from starting point

        printf(">");
        for(int i = 0; i < numChar; i++){ // print chars from starting point to input numChar

            char c = fgetc(filepointer);

            if (c == '\n' || c == '\t' || c == '\r'){ // if not valid character replace it as a space
                printf(" ");
            } else {
                printf("%c", c );
            }     
        }
        printf("<\n");
    }

    fclose(filepointer);
    return 0;
}
  