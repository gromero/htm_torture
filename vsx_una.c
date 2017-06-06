#include <stdio.h>
#include <inttypes.h>

int main() {

asm(

  
   "1: tbegin.;"
   "beq 2f;"
   "xxmrghd 31, 33, 34;" // Exclusive VSX insturction (can not act as Altivec/VMX or FP
   "tend. ;"
   "2: .long 0x0;"
   );

}
