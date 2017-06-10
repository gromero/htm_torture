#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sched.h>


uint64_t worker() {

	uint64_t high; // High 64bit (MSB)
	uint64_t low;  // Low  64bit (LSB)

	uint64_t out;
	
	asm (
		// r3 = 0x5555555555555555
		"lis  %[high], 0x5555                  ;\n"
		"ori  %[high], %[high], 0x5555         ;\n"
		"sldi %[high], %[high], 32             ;\n"
		"oris %[high], %[high], 0x5555         ;\n"
		"ori  %[high], %[high], 0x5555         ;\n"
		
		// r4 = 0xFFFFFFFFFFFFFFFF
		"lis  %[low], 0xFFFF                   ;\n"
		"ori  %[low], %[low], 0xFFFF           ;\n"
		"sldi %[low], %[low], 32               ;\n"
		"oris %[low], %[low], 0xFFFF           ;\n"
		"ori  %[low], %[low], 0xFFFF           ;\n"
				
		// touch vmx
		"mtvsrd 33, %[low]                     ;\n"
		"xxmrghd 32, 33, 34                      ;\n"

		//wait here
                "lis      3, 0x3fff;\n"                                                                           
                "1: ;\n"
                "addi     3, 3, -1;\n"
                "cmpdi    3, 0;\n"
                "bne      1b;\n"

		"1: tbegin.				  ;\n"
		"beq 2f                         	  ;\n"
		"tsuspend.				  ;\n"

		// touch VMX
		"xxmrghd 32, 33, 34                      ;\n"

		// touching FPR
		"lfsx 1, 0, %[low]		;\n"
		"stfd 1, 0(%[out]) 		;\n"

		"tresume. 	;\n"
		"tend.		;\n"

		"2: nop                         ;\n"
		
		: // no  output
		: [out] "r" (out),  [high] "r" (high), [low]  "r" (low)
		:
		);

	return out;
}

int main(){
	uint64_t out;

	out = worker();

	printf("%lx\n", out);

}
