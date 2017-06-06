#include <stdio.h>
#include <inttypes.h>

int main() {

uint64_t high; // High 64bit (MSB)
uint64_t low;  // Low  64bit (LSB)

asm (
   // r3 = 0x5555555555555555
   "lis  %[high], 0x5555                  ;"
   "ori  %[high], %[high], 0x5555         ;"
   "sldi %[high], %[high], 32             ;"
   "oris %[high], %[high], 0x5555         ;"
   "ori  %[high], %[high], 0x5555         ;"

   // r4 = 0xFFFFFFFFFFFFFFFF
   "lis  %[low], 0xFFFF                   ;"
   "ori  %[low], %[low], 0xFFFF           ;"
   "sldi %[low], %[low], 32               ;"
   "oris %[low], %[low], 0xFFFF           ;"
   "ori  %[low], %[low], 0xFFFF           ;"
   "mtvsrd 33, %[high]                    ;"
   "mtvsrd 34, %[low]                     ;"

   // VSR32 (VSX) = VR0 (VMX/Altivec) = 0x5555555555555555555FFFFFFFFFFFFFFFF
   "xxmrghd 32, 33, 34                      ;"

   // Update vmx_correct_value
// "stvx 0, 0, %[vmx_correct_value]       ;"

   : // no  output
// : [vmx_correct_value] "r" (vmx_correct_value),
   : [high] "r" (high),
     [low]  "r" (low)
   :
   );


unsigned int i;
for (i=0; i < 1024*1024*1024; i++);
for (i=0; i < 1024*1024*1024; i++);
// for (i=0; i < 1024*1024*1024; i++);
// for (i=0; i < 1024*1024*1024; i++);

asm(


   "1: tbegin.;"
   "beq 2f;"
// "mtvsrd 32, 3;"
   "xxmrghd 31, 33, 34;"
   "tend. ;"
   "2: .long 0x0;"
   );

}
