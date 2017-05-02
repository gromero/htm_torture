#include <inttypes.h>
#include <htmintrin.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define _ asm
#define DEFAULT_THREADS 16384

void *worker() {
	// VMX registers.
	register vector __int128 vmx0  asm ("vs32");

	vector __int128 vmx_correct_value[32];
	vector __int128 vmx_scratch_area[2];

	uint64_t offset = 0;
	uint64_t z;

	// Expected correct values restored after a HTM failure.
	vmx_correct_value[0]  = (vector __int128) {0xBEEF};

	// Expected correct value restored to VRSAVE after a HTM failure.
	uint64_t vrsave_correct_value[1];     // A place to keep the correct value in memory
	vrsave_correct_value[0] = 0xBABEBEEF; // The correct value itself, a 32-bit value

	// Set values for each VMX register just before entering in a transactional block.
	// Values below must match the expected correct values after a HTM failure,
	// as specified in the code section above. TODO: add random generated value as
	// the expected correct values to torture kernel VMX restore code.
	vmx0  = (vector __int128) {0xBEEF};

	/***************
	** HTM BEGIN **
	****************/
	_ ("tbegin.  \n\t");
	_ goto ("beq %l[_failure] \n\t" : : : : _failure);

	/**************
	** HTM BODY **
	**************/

	// We set VMX register with a value different from what they
	// were set *before* entering the HTM. If the transaction
	// fails, then VMX registers after _failure is taken must
	// be restored back to the values *before* entering the HTM.
	vmx0  = (vector __int128) {0xBABE};


	_ ("nop \n\t");

	/*************
	** HTM END **
	*************/
	_ ("tend.    \n\t");
	_ goto ("b %l[_success] \n\t" : : : : _success);


_failure:
	  _ goto (

	    // Check if vmx0 is sane.
	    "stvx 1, 0, %[vmx_scratch_area]   \n\t"
	    "lvx  1, 0, %[vmx_correct_value]  \n\t"
	    "vcmpequb. 0, 0, 1                \n\t"
	    "lvx  1, 0,  %[vmx_scratch_area]  \n\t"
	    "mfvrd 5, 0                       \n\t"
	    "li     6, 0                      \n\t"
	    "bc   4, 24, %l[_value_mismatch]  \n\t"

	    // Reach here, then all registers are sane.
	    "b  %l[_value_match]              \n\t"

	       : // no output
	       : [offset] "r"(offset),
	         [vmx_correct_value] "r"(vmx_correct_value),
	         [vmx_scratch_area]  "r"(vmx_scratch_area),
	         [vrsave_correct_value] "r"(vrsave_correct_value)
	       : "memory", "r5", "r6"
	       : _value_match, _value_mismatch
	    );

_value_mismatch:
	z = __builtin_get_texasr();
	printf("\n\n==============\n\n");
	printf("\nFailure with error: %lx\n\n",  _TEXASR_FAILURE_CODE(z));
	printf(": Summary error: %lx\n",  _TEXASR_FAILURE_SUMMARY(z));
	printf(": TFIAR error: %lx\n\n", _TEXASR_TFIAR_EXACT(z));

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

	printf("Torture tool starting with %d threads\n", threads);

	pthread_t thread[threads];
	for (uint64_t i = 0; i < threads; i++)
		pthread_create(&thread[i], NULL, &worker, NULL);

	for (uint64_t i = 0; i < threads; i++)
		pthread_join(thread[i], NULL);

	return 0;
}
