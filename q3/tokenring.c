#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>
#define MAX_STR_SIZE 10000

int main(int argc, char* argv[]){
    int token = 0;
    srand(time(NULL));
    char* pipenames = (char*)malloc(MAX_STR_SIZE*sizeof(char));
    
    for (int i = 1; i < atoi(argv[1]); i++){
        sprintf(pipenames, "/tmp/pipe%dto%d", i, i + 1);
        mkfifo(pipenames, 0666);
    }
    sprintf(pipenames, "/tmp/pipe%dto%d", atoi(argv[1]), 1);
    mkfifo(pipenames, 0666);

    for (int i = 0; i < atoi(argv[1])-1; i++){
            pid_t pid = fork();
            if(pid<0){
                perror("fork error");
                exit(EXIT_FAILURE);
            }
            if (pid == 0){
                if (i + 2 == atoi(argv[1])){
                    char read_pipe[50];
                    char write_pipe[50];
                    sprintf(read_pipe, "/tmp/pipe%dto%d", i+1, i+2);
                    sprintf(write_pipe, "/tmp/pipe%dto%d", i+2, 1);
                    int fd_read = open(read_pipe, O_RDONLY);
                    int fd_write = open(write_pipe, O_WRONLY);
                    while (read(fd_read, &token, sizeof(token)) > 0){
                        token++;
                        float r = ((float)rand()/(float)(RAND_MAX));
                        if (r < atof(argv[2])){
                            printf("[p%d] lock on token (val = %d)\n", i+1, token);
                            sleep(atof(argv[3]));
                            printf("[p%d] unlock token\n", i+1);
                        }
                        write(fd_write, &token, sizeof(token));
                    }
                    close(fd_read);
                    close(fd_write);
                }
                else{
                    char read_pipe[50];
                    char write_pipe[50];
                    sprintf(read_pipe, "/tmp/pipe%dto%d", i+1, i+2);
                    sprintf(write_pipe, "/tmp/pipe%dto%d", i+2, i+3);
                    int fd_read = open(read_pipe, O_RDONLY);
                    int fd_write = open(write_pipe, O_WRONLY);                  
                    while (read(fd_read, &token, sizeof(token)) > 0){
                        token++;
                        float r = ((float)rand()/(float)(RAND_MAX));
                        if (r < atof(argv[2])){
                            printf("[p%d] lock on token (val = %d)\n", i+1, token);
                            sleep(atof(argv[3]));
                            printf("[p%d] unlock token \n", i+1);
                        }
                        write(fd_write, &token, sizeof(token));
                    }
                    close(fd_read);
                    close(fd_write);
                }
                exit(EXIT_SUCCESS);
            }
        }


    char read_pipe[50];
    char write_pipe[50];
    sprintf(read_pipe, "/tmp/pipe%dto%d", atoi(argv[1]), 1);
    sprintf(write_pipe, "/tmp/pipe%dto%d", 1, 2);
    int fd_write = open(write_pipe, O_WRONLY);
    int fd_read = open(read_pipe, O_RDONLY);
    write(fd_write, &token, sizeof(token));
    while (read(fd_read, &token, sizeof(token)) > 0){
        token++;
        float r = ((float)rand()/(float)(RAND_MAX));
        if (r < atof(argv[2])){
            printf("[p%d] lock on token (val = %d)\n", atoi(argv[1]), token);
            sleep(atof(argv[3]));
            printf("[p%d] unlock\n", atoi(argv[1]));
        }
        write(fd_write, &token, sizeof(token));
    }
    close(fd_read);
    close(fd_write);
    return 0;
}