#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}

// checks if number
static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}

int main(int c,char**v){
 if (c != 2) {
    fprintf(stderr, "Usage: %s <pid>\n", v[0]);
    return 1;
}

// make sure pid is a number
for (char *p = v[1]; *p; p++) {
    if (!isdigit(*p)) {
        fprintf(stderr, "Error: PID must be numeric\n");
        return 1;
    }
}

 int pid = atoi(v[1]);
 char path[256];
 FILE *f;
 
 // read for stats
 snprintf(path, sizeof(path), "/proc/%d/stat", pid);
 f = fopen(path, "r");
 if (!f) {
    perror("Error");
    return 1;
 }
 
 int pid_read;
 int ppid;
 char comm[256];
 char state;
 unsigned long utime;
 unsigned long stime;
 
 fscanf(f, "%d %s %c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu", &pid_read, comm, &state, &ppid, &utime, &stime);
 fclose(f);
 
 double cpu = (utime + stime) / (double)sysconf(_SC_CLK_TCK);
 
 printf("PID: %d\n", pid_read);
 printf("State: %c\n", state);
 printf("PPID: %d\n", ppid);
 
 // read  for memory
 snprintf(path, sizeof(path), "/proc/%d/status", pid);
 f = fopen(path, "r");
 
 if (f) {
    char line[256];
    int vmrss = 0;
    while (fgets (line, sizeof(line), f)){
        if (sscanf(line, "VmRSS: %d", &vmrss) == 1) break;
    }
    fclose(f);
    printf("VmRSS: %d\n", vmrss);
    }
    
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    f = fopen(path, "r");
    
    if (f) {
        char cmd[1024];
        size_t len = fread(cmd, 1, sizeof(cmd) - 1, f);
        cmd[len] = '\0';
      
        for (size_t i = 0; i < len; i++) {
            if (cmd[i] == '\0') cmd[i] = ' ';
        }
        
        fclose(f);
        printf("Cmd: %s\n", cmd);
    }
    
    printf("CPU: %.2f\n", cpu);
    
    return 0;
}
/* outputs: need a getter for each of these
spit out the PID input 
what state its in state
who is its parent PPID
what command its running Cmd: /usr/bin/firefoc
how much cpu its using CPU: 15.23
how much memory its using VmRSS: 524288
*/