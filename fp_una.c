#include <stdio.h>
#include <inttypes.h>

int main() {

asm(
  
   "1: tbegin.;"
   "beq 2f;"
   "mtvsrd 0, 3;" // mtvsrd is am VSX instruction but vs0 is also f0, so it generates "FP Unavailable..."
   "tend. ;"
   "2: nop ;"
   );

}
