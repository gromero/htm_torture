#include <inttypes.h>
#include <htmintrin.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/mman.h>

#define _ asm

#define DEFAULT_THREADS 1

void *worker() {

        void *vmx0_correct_value = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        void *vmx0_new_value     = mmap(NULL, 16 /* 128 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      
        void *fp_correct_value = mmap(NULL, 8 /* 64 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	void *fp_new_value = mmap(NULL, 8 /* 64 bits */, PROT_READ | PROT_WRITE,  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        uint64_t _texasr;

	_  goto (
           "xor     3,  3, 3                      \n\t"
	   "ori     3,  3, 48879                  \n\t" // 0xBEEF
           "std     3,  0(%[vmx0_correct_value])  \n\t"
           "std     3,  0(%[fp_correct_value])    \n\t" // Save 0xBEEF also on fp
	   "lvx     0,  0, %[vmx0_correct_value]  \n\t" // vmx0 = 0x0000000000000000000000000000BEEF

           "xor      3,  3, 3                      \n\t"
	   "ori      3,  3, 47806                  \n\t" // 0xBABE
	   "std      3,  0(%[vmx0_new_value])      \n\t"

	/**************
	** DELAY     **
	***************/

	   "lis	     3, 0x3fff\n\t"
	   "1:\n\t"
	   "addi     3, 3, -1\n\t"
	   "cmpdi    3, 0\n\t"
	   "bne	     1b\n\t"

	/**************
	** SET FPR=1  *
	***************/
	   "lfd 0,  0(%[fp_correct_value])\n\t"

	/**************
	** HTM BEGIN **
	***************/

	   "tbegin.                               \n\t"
	   "beq     _failure                      \n\t"

	/**************
	** HTM suspend **
	***************/
	   "tsuspend. \n\t"

	/**************
	** Touch VMX on suspended mode *
	***************/
	   "lvx     0,  0, %[vmx0_correct_value]  \n\t" // vmx0 = 0x0000000000000000000000000000BEEF

	/**************
	** Check if FPR is correct
	***************/
	   "stfd 0,  0(%[fp_new_value])\n\t"

	/*************
	** HTM RESUME **
	**************/
           
           "tresume. \n\t"


	/************
	** HTM END **
	*************/

           "tend.                                 \n\t"
	   "b     %l[_success]                    \n\t"

	    // Check vmx0 sanity
	    "_failure: lfd       9,  0 (%[fp_correct_value])     \n\t"
	    "          fcmpu     0, 0, 9                         \n\t"
	    "          bne  %l[_value_mismatch]       	  	\n\t"
	    "          b  %l[_value_match]                        \n\t"
	   :
	   : [vmx0_correct_value] "r"(vmx0_correct_value),
             [vmx0_new_value]     "r"(vmx0_new_value),
             [fp_correct_value]   "r"(fp_correct_value),
             [fp_new_value]   "r"(fp_new_value)
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
}

int main(int argc, char **argv){
	int threads = DEFAULT_THREADS;

	if (argc > 1)
		threads = atoi(argv[1]);

	printf("Torture starting with %d threads.\n", threads);

	//pthread_t thread[threads];
	//for (uint64_t i = 0; i < threads; i++)
	//	pthread_create(&thread[i], NULL, &worker, NULL);

	//for (uint64_t i = 0; i < threads; i++)
	//	pthread_join(thread[i], NULL);
	//

	worker();

	printf("\n");

	return 0;
}
