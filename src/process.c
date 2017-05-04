#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <inttypes.h>
#include <htmintrin.h>
#include <string.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/wait.h>
#include "torture.h"

#define SHMSZ        16 // 128-bit
#define NUM_CHILDREN 24  // total number of processes

int start_worker_process(int workload, int nr_processes)
 {   
     char c;
     int shmid;
     key_t key; 
     char *shm, *s;

     int pid;     
     uint64_t _texasr;

     key = 5678;

//     void *vmx0_correct_value = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//     void *vmx0_new_value     = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
/*     
     if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
         perror("shmget");
         exit(1);
     }
     
     if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
         perror("shmat");
         exit(1);
     }
     
     s = shm;
*/ 
    int num_children;
    for (num_children = 0; num_children < nr_processes; num_children++) {
 
     pid = fork();

     if (pid != 0) { // it's a parent
     } else { // it's a child.
       // printf("I'm a child...\n");
       break;
     }
     }
     if (num_children != nr_processes) {

//     printf("workload%d\n", workload);
//     printf("counter = %d\n", num_children);
     worker((void *)workload);
     } 
     return 0;
 }
