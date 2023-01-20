#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define MAX_ARGS 32 

char* epubExtension(char* argv){

    char* result = (char*)malloc(MAX_ARGS*sizeof(char)); 

    for(int i= 0; i< strlen(argv)-4; i++){
        result[i] = argv[i];
    }

    strcat(result, ".epub");

    return result;
}

int main(int argc, char* argv[]) {

    // check if arguments are present
    if(argc < 2) {
        perror("Incorrect usage. Not enough arguments.\n");
        return EXIT_FAILURE;
    }

    // check if files can be opened 
    for (int i = 1; i < argc; i++){

        int fd = open(argv[i], O_RDONLY);

        if (fd == -1) {
            printf("error: cannot open %s\n", argv[i]);
            return EXIT_FAILURE;
        }

        close(fd);
    }

    char* args[MAX_ARGS];
    char* epubs[argc - 1];

    for (int i = 1; i < argc; i++){

        args[0] = "/usr/bin/pandoc";            
        args[1] = argv[i];
        args[2] = "-o";
        args[3] = epubExtension(argv[i]);
        args[4]= "--quiet"; // to avoid printing the pandoc output
        args[5] = NULL;

        // store in epubs array epub file names
        epubs[i-1] = args[3];
        
        int pid = fork();

        if(pid < 0){

            perror("error on fork");
            exit(EXIT_FAILURE);

        } else if (pid == 0){

            printf("[pid%d] converting %s ...\n", getpid(), argv[i]);

            if(execvp(args[0], args) < 0){
                perror("file to epub conversion failed");
                exit(EXIT_FAILURE);
            }
        }

    }
        
    for(int i = 1 ; i < argc ; i++ ){
        if( wait(NULL) == -1) {  
            perror("error on waiting for child processes");
            exit(EXIT_FAILURE);
        }
    }
    
    // creating zip file
    char *zip[argc + 4]; 

    zip[0] = "/usr/bin/zip";
    zip[1] = "ebooks.zip";

    for(int i = 0; i < argc - 1; i++){
        zip[i + 2] = epubs[i]; 
    }

    zip[argc + 1]= "--quiet"; // to avoid printing the zip output
    zip[argc + 2] = NULL;

    execvp(zip[0], zip);

    return 0;
}