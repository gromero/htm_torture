#include <inttypes.h>

int sleeper() {
	uint64_t i;
	for(i=0; i<1024*1024*512; i++); // wait an amount of context switches so load_fp and load_vec overflows and MSR.[FP|VEC|VSX] = 0
}
