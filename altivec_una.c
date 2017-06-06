#include <stdio.h>
#include <inttypes.h>

int main() {

asm(
  
   "1: tbegin.;"
   "beq 2f;"
   "mtvsrd 32, 3;" // mtvsrd is a VSX instruction but vs32 is also vr0, so it generates "Vector Unavailable..."
   "tend. ;"
   "2: nop ;"
   );

}
