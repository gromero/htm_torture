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


#define SHMSZ        16 // 128-bit
#define NUM_CHILDREN 24  // total number of processes

#define _ asm

 int main()
 {   
     char c;
     int shmid;
     key_t key; 
     char *shm, *s;

     int pid;     
     uint64_t _texasr;

     key = 5678;

     void *vmx0_correct_value = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
     void *vmx0_new_value     = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
     
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

     if (pid != 0) { // it's a parent
     } else { // it's a child. b
       printf("I'm a child...\n");
       break;
     }
     }
printf("HTM\n");
       // HTM goes in here
while(1) {
	_  goto (
           "xor     3,  3, 3                      \n\t"
	   "ori     3,  3, 48879                  \n\t" // 0xBEEF
           "std     3,  0(%[vmx0_correct_value])  \n\t"
	   "lvx     0,  0, %[vmx0_correct_value]  \n\t" // vmx0 = 0x0000000000000000000000000000BEEF

           "xor      3,  3, 3                      \n\t"
	   "ori      3,  3, 47806                  \n\t" // 0xBABE
	   "std      3,  0(%[s])      \n\t"
/*
	   "lis	     3, 0x3fff\n\t"
	   "1:\n\t"
	   "addi     3, 3, -1\n\t"
	   "cmpdi    3, 0\n\t"
	   "bne	     1b\n\t"
*/
	/**************
	** HTM BEGIN **
	***************/

	   "tbegin.                               \n\t"
	   "beq     _failure                      \n\t"


	/*************
	** HTM BODY **
	**************/

           "lvx  0, 0, %[s]          \n\t"


	/************
	** HTM END **
	*************/

           "tend.                                 \n\t"
	   "b     %l[_success]                    \n\t"

	    // Check vmx0 sanity
	    "_failure: lvx       1,  0, %[vmx0_correct_value]     \n\t"
	    "          vcmpequb. 2,  0, 1                         \n\t"
	    "          bc        4, 24, %l[_value_mismatch]       \n\t"
	    "          b  %l[_value_match]                        \n\t"
	   :
	   : [vmx0_correct_value] "r"(vmx0_correct_value),
//           [vmx0_new_value]     "r"(vmx0_new_value)
             [s] "r"(s)
	   : "r3"
	   : _success, _value_match, _value_mismatch
	  );

// HTM failed and vmx is corrupted
_value_mismatch:
	_texasr = __builtin_get_texasr();

	printf("\n\n==============\n\n");
	printf("Failure with error: %lx\n",   _TEXASR_FAILURE_CODE(_texasr));
	printf("Summary error     : %lx\n",   _TEXASR_FAILURE_SUMMARY(_texasr));
	printf("TFIAR exact       : %lx\n\n", _TEXASR_TFIAR_EXACT(_texasr));

	_ (".long 0"); // Exit with a core dump

// HTM failed but vmx is not corrputed
_value_match:
	printf("!");
	goto _finish;

// HTM succeeded
_success:
	printf(".");

_finish:
	_ ("nop"); // a 'nop' just because we can't label in the void...
        fflush(0);
} // while(1)

     exit(0);
 }
