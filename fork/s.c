#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHMSZ        16 // 128-bit
#define NUM_CHILDREN 7  // total number of processes

 int main()
 {   
     char c;
     int shmid;
     key_t key; 
     char *shm, *s;

     int pid;     

     key = 5678;
     
     if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
         perror("shmget");
         exit(1);
     }
     
     if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
         perror("shmat");
         exit(1);
     }
     
     s = shm;
 
    for (int num_children=0; num_children < NUM_CHILDREN; num_children++) {
 
     pid = fork();

     if (pid == 0) { // it's a parent
      if (num_children == 0) { // then it's yet a parent of a single child
       printf("I'm a parent");
       // Add parent main activity here

      }
      break;

/*
       for (c = 'a'; c <= 'z'; c++)
         *s++ = c;
       *s = NULL;
     
       while (*shm != '*')
         sleep(1);
*/

     } else { // it's a child. Execute TM.
       printf("I'm a child...\n");

       // HTM goes in here
/*
         for (s = shm; *s != NULL; s++)
           putchar(*s);
         putchar('\n');

         *shm = '*';
*/
     } // else
     } // for 

     exit(0);
 }
