#include <stdio.h>
#include<htmintrin.h>
#include <sys/types.h>
#include <unistd.h> 
#include <inttypes.h>

int main()
{
 register vector __int128 vsx asm("vs0");
 uint64_t aux; // GPRx
 
 vsx = (vector __int128) {0xBEEF}; 
 asm(
//   "vsldoi  0, 0, 0, 12;" // nah, it just changes VMX!
     "xxsldwi 0, 0, 0, 2;"
     "mfvsrd %[aux], 0  ;"
     :  [aux] "=r"(aux) 
     : 
     : 
     );

 printf("%p\n", (void *)aux);
}
