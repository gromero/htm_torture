#include <inttypes.h>
#include <htmintrin.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>

#define _ asm
#define DEFAULT_THREADS 16384

void *worker() {

        void *vmx0_correct_value = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        void *vmx0_new_value     = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      
        uint64_t _texasr;

	_  goto (
           "xor     3,  3, 3                      \n\t"
	   "ori     3,  3, 48879                  \n\t" // 0xBEEF
           "std     3,  0(%[vmx0_correct_value])  \n\t"
	   "lvx     0,  0, %[vmx0_correct_value]  \n\t" // vmx0 = 0x0000000000000000000000000000BEEF

           "xor      3,  3, 3                      \n\t"
	   "ori      3,  3, 47806                  \n\t" // 0xBABE
	   "std      3,  0(%[vmx0_new_value])      \n\t"

	/***************
	** HTM BEGIN **
	****************/

	   "tbegin.                               \n\t"
	   "beq     %l[_failure]                  \n\t"
           
           "lvx  0, 0, %[vmx0_new_value]          \n\t"
           "tend.                                 \n\t"
	   "b     %l[_success]                    \n\t"

	   :
	   : [vmx0_correct_value] "r"(vmx0_correct_value),
             [vmx0_new_value]     "r"(vmx0_new_value)
	   : "r3"	     
	   : _failure, _success
	  );



_failure:
	  _ goto (

	    // Check if vmx0 is sane.
	    "lvx  1, 0, %[vmx0_correct_value]  \n\t"
	    "vcmpequb. 2, 0, 1                \n\t"
	    "bc   4, 24, %l[_value_mismatch]  \n\t"

	    // Reach here, then all registers are sane.
	    "b  %l[_value_match]              \n\t"

	       : // no output
	       : [vmx0_correct_value] "r" (vmx0_correct_value) 
	       : 
	       : _value_match, _value_mismatch
	    );

_value_mismatch:
	_texasr = __builtin_get_texasr();
	printf("\n\n==============\n\n");
	printf("\nFailure with error: %lx\n\n",  _TEXASR_FAILURE_CODE(_texasr));
	printf(": Summary error: %lx\n",  _TEXASR_FAILURE_SUMMARY(_texasr));
	printf(": TFIAR error: %lx\n\n", _TEXASR_TFIAR_EXACT(_texasr));

	_ (".long 0"); // exit with a core dump

_value_match:
	printf("!");
	goto _finish;

_success:
	printf(".");

_finish:
	_ ("nop"); // Can't label in the void...

}

int main(int argc, char **argv){
	int threads = DEFAULT_THREADS;

	if (argc > 1)
		threads = atoi(argv[1]);
        //worker();

	printf("Torture tool starting with %d threads\n", threads);

	pthread_t thread[threads];
	for (uint64_t i = 0; i < threads; i++)
		pthread_create(&thread[i], NULL, &worker, NULL);

	for (uint64_t i = 0; i < threads; i++)
		pthread_join(thread[i], NULL);

		return 0;
}
