#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sched.h>



float work(float a) {
	float b;


	asm volatile(
		"lfs 9, 0(%[a])	;\n"
		"xxmrghd 32, 33, 34 ;\n"

                //wait here
                "lis      3, 0x4fff;\n"                                                                           
                "1: ;\n"
                "addi     3, 3, -1;\n"
                "cmpdi    3, 0;\n"
                "bne      1b;\n"

		"fmr %[b], 9 ;\n"

		: [b] "=f" (b)
		: [a] "r" (&a)
		:
	   );

	return b;
}

int main(){

	int a = 3.14;
	float out ;
	pid_t p;

	p = fork();

	if (p)
		a = 3.14;
	else
		a = 2.42;

	while (1){
		out = work(a);
		if (out != a)	
			printf("Wrong value: %f\n", out);
		else {
			if (p)
				printf(".");
			else
				printf("!");
		fflush(stdout);
		}
	}
	return 0;
}
