#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <altivec.h>

#define _ asm
#define __ volatile

void workload0() {
        printf("Workload 0\n");
}

int main(int argc, char **argv)
{
 // VSX registers. To be used in the future.
 register vector __int128 vsx0  asm ("vs0");
 register vector __int128 vsx1  asm ("vs1");

 // VMX registers.
 register vector __int128 vmx0  asm ("vs32");
 register vector __int128 vmx1  asm ("vs33");
 register vector __int128 vmx2  asm ("vs34");
 register vector __int128 vmx3  asm ("vs35");
 register vector __int128 vmx4  asm ("vs36");
 register vector __int128 vmx5  asm ("vs37");
 register vector __int128 vmx6  asm ("vs38");
 register vector __int128 vmx7  asm ("vs39");
 register vector __int128 vmx8  asm ("vs40");
 register vector __int128 vmx9  asm ("vs41");
 register vector __int128 vmx10 asm ("vs42");
 register vector __int128 vmx11 asm ("vs43");
 register vector __int128 vmx12 asm ("vs44");
 register vector __int128 vmx13 asm ("vs45");
 register vector __int128 vmx14 asm ("vs46");
 register vector __int128 vmx15 asm ("vs47");
 register vector __int128 vmx16 asm ("vs48");
 register vector __int128 vmx17 asm ("vs49");
 register vector __int128 vmx18 asm ("vs50");
 register vector __int128 vmx19 asm ("vs51");
 register vector __int128 vmx20 asm ("vs52");
 register vector __int128 vmx21 asm ("vs53");
 register vector __int128 vmx22 asm ("vs54");
 register vector __int128 vmx23 asm ("vs55");
 register vector __int128 vmx24 asm ("vs56");
 register vector __int128 vmx25 asm ("vs57");
 register vector __int128 vmx26 asm ("vs58");
 register vector __int128 vmx27 asm ("vs59");
 register vector __int128 vmx28 asm ("vs60");
 register vector __int128 vmx29 asm ("vs61");
 register vector __int128 vmx30 asm ("vs62");
 register vector __int128 vmx31 asm ("vs63");

 vector __int128 vmx_correct_value[32];
 vector __int128 vmx_scratch_area[2];

 uint64_t offset = 0;

 // Expected correct values restored after a HTM failure.
 vmx_correct_value[0]  = (vector __int128) {0xBEEF};
 vmx_correct_value[1]  = (vector __int128) {0xBEEF};
 vmx_correct_value[2]  = (vector __int128) {0xBEEF};
 vmx_correct_value[3]  = (vector __int128) {0xBEEF};
 vmx_correct_value[4]  = (vector __int128) {0xBEEF};
 vmx_correct_value[5]  = (vector __int128) {0xBEEF};
 vmx_correct_value[6]  = (vector __int128) {0xBEEF};
 vmx_correct_value[7]  = (vector __int128) {0xBEEF};
 vmx_correct_value[8]  = (vector __int128) {0xBEEF};
 vmx_correct_value[9]  = (vector __int128) {0xBEEF};
 vmx_correct_value[10] = (vector __int128) {0xBEEF};
 vmx_correct_value[11] = (vector __int128) {0xBEEF};
 vmx_correct_value[12] = (vector __int128) {0xBEEF};
 vmx_correct_value[13] = (vector __int128) {0xBEEF};
 vmx_correct_value[14] = (vector __int128) {0xBEEF};
 vmx_correct_value[15] = (vector __int128) {0xBEEF};
 vmx_correct_value[16] = (vector __int128) {0xBEEF};
 vmx_correct_value[17] = (vector __int128) {0xBEEF};
 vmx_correct_value[18] = (vector __int128) {0xBEEF};
 vmx_correct_value[19] = (vector __int128) {0xBEEF};
 vmx_correct_value[20] = (vector __int128) {0xBEEF};
 vmx_correct_value[21] = (vector __int128) {0xBEEF};
 vmx_correct_value[22] = (vector __int128) {0xBEEF};
 vmx_correct_value[23] = (vector __int128) {0xBEEF};
 vmx_correct_value[24] = (vector __int128) {0xBEEF};
 vmx_correct_value[25] = (vector __int128) {0xBEEF};
 vmx_correct_value[26] = (vector __int128) {0xBEEF};
 vmx_correct_value[27] = (vector __int128) {0xBEEF};
 vmx_correct_value[28] = (vector __int128) {0xBEEF};
 vmx_correct_value[29] = (vector __int128) {0xBEEF};
 vmx_correct_value[30] = (vector __int128) {0xBEEF};
 vmx_correct_value[31] = (vector __int128) {0xBEEF};

 // Values for each VMX register just *before* entering in a transactional block.
 // Values below must match the expected correct values after a HTM failure, as
 // as specified in the code section above. TODO: add random generation value as
 // the expected correct values to torture kernel VMX restore code. 
 vmx0 = (vector __int128) {0xBEEF};
 vmx1 = (vector __int128) {0xBEEF};
 vmx2 = (vector __int128) {0xBEEF};
 vmx3 = (vector __int128) {0xBEEF};
 vmx4 = (vector __int128) {0xBEEF};
 vmx5 = (vector __int128) {0xBEEF};
 vmx6 = (vector __int128) {0xBEEF};
 vmx7 = (vector __int128) {0xBEEF};
 vmx8 = (vector __int128) {0xBEEF};
 vmx9 = (vector __int128) {0xBEEF};
 vmx10 = (vector __int128) {0xBEEF};
 vmx11 = (vector __int128) {0xBEEF};
 vmx12 = (vector __int128) {0xBEEF};
 vmx13 = (vector __int128) {0xBEEF};
 vmx14 = (vector __int128) {0xBEEF};
 vmx15 = (vector __int128) {0xBEEF};
 vmx16 = (vector __int128) {0xBEEF};
 vmx17 = (vector __int128) {0xBEEF};
 vmx18 = (vector __int128) {0xBEEF};
 vmx19 = (vector __int128) {0xBEEF};
 vmx20 = (vector __int128) {0xBEEF};
 vmx21 = (vector __int128) {0xBEEF};
 vmx22 = (vector __int128) {0xBEEF};
 vmx23 = (vector __int128) {0xBEEF};
 vmx24 = (vector __int128) {0xBEEF};
 vmx25 = (vector __int128) {0xBEEF};
 vmx26 = (vector __int128) {0xBEEF};
 vmx27 = (vector __int128) {0xBEEF};
 vmx28 = (vector __int128) {0xBEEF};
 vmx29 = (vector __int128) {0xBEEF};
 vmx30 = (vector __int128) {0xBEEF};
 vmx31 = (vector __int128) {0xBEEF};

 _ ("tbegin.  \n\t");
 _ goto ("beq %l[_failure] \n\t" : : : : _failure);

 // We set VMX register with a value different from what they
 // were set *before* entering the HTM. If the transaction
 // fails, then VMX registers after _failure is taken must
 // be restored back to the values *before* entering the HTM.

 vmx0 = (vector __int128) {0xBABE};
 vmx1 = (vector __int128) {0xBABE};
 vmx2 = (vector __int128) {0xBABE};
 vmx3 = (vector __int128) {0xBABE};
 vmx4 = (vector __int128) {0xBABE};
 vmx5 = (vector __int128) {0xBABE};
 vmx6 = (vector __int128) {0xBABE};
 vmx7 = (vector __int128) {0xBABE};
 vmx8 = (vector __int128) {0xBABE};
 vmx9 = (vector __int128) {0xBABE};
 vmx10 = (vector __int128) {0xBABE};
 vmx11 = (vector __int128) {0xBABE};
 vmx12 = (vector __int128) {0xBABE};
 vmx13 = (vector __int128) {0xBABE};
 vmx14 = (vector __int128) {0xBABE};
 vmx15 = (vector __int128) {0xBABE};
 vmx16 = (vector __int128) {0xBABE};
 vmx17 = (vector __int128) {0xBABE};
 vmx18 = (vector __int128) {0xBABE};
 vmx19 = (vector __int128) {0xBABE};
 vmx20 = (vector __int128) {0xBABE};
 vmx21 = (vector __int128) {0xBABE};
 vmx22 = (vector __int128) {0xBABE};
 vmx23 = (vector __int128) {0xBABE};
 vmx24 = (vector __int128) {0xBABE};
 vmx25 = (vector __int128) {0xBABE};
 vmx26 = (vector __int128) {0xBABE};
 vmx27 = (vector __int128) {0xBABE};
 vmx28 = (vector __int128) {0xBABE};
 vmx29 = (vector __int128) {0xBABE};
 vmx30 = (vector __int128) {0xBABE};
 vmx31 = (vector __int128) {0xBABE};

  _ ("tabort. 0 \n\t");

 _ ("tend.    \n\t");

 _ goto ("b %l[_success] \n\t" : : : : _success);

_failure:
   _ goto (
     // Check if vmx0 is sane.
     "stvx 1, 0, %[vmx_scratch_area]   \n\t"
     "lvx  1, 0, %[vmx_correct_value]  \n\t"
     "vcmpequb. 0, 0, 1                \n\t"
     "lvx  1, 0,  %[vmx_scratch_area]  \n\t"
     "bc   4, 24, %l[_value_mismatch]  \n\t"

     // Check if vmx1 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value]  \n\t"
     "vcmpequb. 0, 0, 1                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"
 
     // Check if vmx2 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value]  \n\t"
     "vcmpequb. 0, 0, 2                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"  

     // Check if vmx3 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 3                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx4 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 4                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx5 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 5                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx6 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 6                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx7 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 7                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx8 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 8                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx9 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 9                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx10 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 10               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx11 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 11               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx12 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 12                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx13 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 13               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx14 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 14               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx15 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 15               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx16 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 16               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx17 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 17               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx18 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 18               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx19 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 19               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx20 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 20               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx21 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 21               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx22 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 22               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx23 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 23               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx24 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 24               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx25 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 25               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx26 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 26               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx27 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 27                \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx28 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 28               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx29 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 29               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx30 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 30               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Check if vmx31 is sane.
     "addi %[offset], %[offset], 16    \n\t"
     "lvx  0, %[offset], %[vmx_correct_value] \n\t"
     "vcmpequb. 0, 0, 31               \n\t"
     "bc 4, 24, %l[_value_mismatch]    \n\t"

     // Reach here, then all registers are sane.
     "b  %l[_value_match]              \n\t"

	: // no output
        : [offset] "r"(offset),
	  [vmx_correct_value] "r"(vmx_correct_value),
          [vmx_scratch_area]  "r"(vmx_scratch_area)
	: "memory"
	: _value_match, _value_mismatch
     );

_value_mismatch:
        printf("HTM failed and VMX registers got corrupted!\n");
	exit(13);


// salva vmx 1 na memoria
// traz o valor correto para vmx 1
// compara vmx 0 com vmx 1 (valor correto)
// restaura o valor vmx 1 da memoria
//
// traz valor correto de vmx 1 para vmx 0
// compara vmx 1 com vmx 0
//
// traz valor correto de vmx 2 para vmx 0
// compara vmx 2 com vmx 0
//
// ...
//
_value_match:
	printf("HTM failed but VMX registers are OK\n");
	exit(1);

_success:
	printf("HTM succeeded\n");
	exit(0);
}
