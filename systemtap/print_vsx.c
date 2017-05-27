#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

int main()
{
 register vector __int128 vsx asm("vs0");
 uint64_t high_chunk; // GPRx
 uint64_t low_chunk;  // GPRy

 vsx = (vector __int128) {0xBEEF};
 asm(
//   "vsldoi  0, 0, 0, 12;" // nah, it just changes VMX!
     "mfvsrd %[high_chunk], 0;"
     "xxsldwi 0, 0, 0, 2;"
     "mfvsrd %[low_chunk], 0  ;"
     :  [high_chunk] "=r"(high_chunk),
        [low_chunk]  "=r"(low_chunk)
     :

     );

 printf("0x%.16lx%.16lx\n", high_chunk, low_chunk);
}
