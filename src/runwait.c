#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }
    
    struct timespec start, end;
    
    // get start time
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    
    if (pid == 0) {
        // child process - run the command
        execvp(argv[1], &argv[1]);
        perror("execvp failed");
        exit(1);
    }
    
    // parent process - wait for child
    int status;
    waitpid(pid, &status, 0);
    
    // get end time
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    // calculate elapsed time in seconds
    double elapsed = (end.tv_sec - start.tv_sec) + 
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    
    // print results based on how child exited
    if (WIFEXITED(status)) {
        printf("pid=%d elapsed=%.3f exit=%d\n", pid, elapsed, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("pid=%d elapsed=%.3f signal=%d\n", pid, elapsed, WTERMSIG(status));
    }
    
    return 0;
}