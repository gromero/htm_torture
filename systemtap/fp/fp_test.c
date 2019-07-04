/* 
 * Example: 
 * $ gcc -g -O0 fp_test.c -o fp_test -static
 * $ sudo ~/systemtap/bin/stap -B CONFIG_MODVERSIONS=y -d /lib/powerpc64le-linux-gnu/libc-2.24.so -g -k -v -r /home/gromero/git/linux -c "/home/gromero/fp_test" ./probe.stp
 *
 */

/*

 Object should be like the following so probe attaches fine:

 Dump of assembler code for function a_func:     <-- Probe 0
   0x0000000010000878 <+0>:	lis     r2,4107
   0x000000001000087c <+4>:	addi    r2,r2,32000
   0x0000000010000880 <+8>:	mflr    r0
   0x0000000010000884 <+12>:	std     r0,16(r1)
   0x0000000010000888 <+16>:	std     r31,-8(r1)
   0x000000001000088c <+20>:	stdu    r1,-128(r1)
   0x0000000010000890 <+24>:	mr      r31,r1
   0x0000000010000894 <+28>:	mr      r9,r3
   0x0000000010000898 <+32>:	stw     r9,96(r31)
   0x000000001000089c <+36>:	lwa     r9,96(r31)
   0x00000000100008a0 <+40>:	mr      r4,r9
   0x00000000100008a4 <+44>:	addis   r3,r2,-3
   0x00000000100008a8 <+48>:	addi    r3,r3,-22456
   0x00000000100008ac <+52>:	bl      0x10009248 <printf+8>
   0x00000000100008b0 <+56>:	nop
   0x00000000100008b4 <+60>:	li      r3,2816
   0x00000000100008b8 <+64>:	mtvsrd  vs0,r3 // 0xb00
   0x00000000100008bc <+68>:	addi    r3,r3,1   <-- Probe 1
   0x00000000100008c0 <+72>:	mtvsrd  vs1,r3 // 0xb01
   0x00000000100008c4 <+76>:	addi    r3,r3,1   <-- Probe 2
   0x00000000100008c8 <+80>:	li      r4,3054
   0x00000000100008cc <+84>:	mtvsrd  vs2,r4 // 0xbee
   0x00000000100008d0 <+88>:	nop               <-- Probe 3
   0x00000000100008d4 <+92>:	addi    r1,r31,128
   0x00000000100008d8 <+96>:	ld      r0,16(r1)
   0x00000000100008dc <+100>:	mtlr    r0
   0x00000000100008e0 <+104>:	ld      r31,-8(r1)
   0x00000000100008e4 <+108>:	blr
   0x00000000100008e8 <+112>:	.long 0x0
   0x00000000100008ec <+116>:	.long 0x1000000
   0x00000000100008f0 <+120>:	.long 0x1000180
End of assembler dump.
*/



#include <stdio.h>

void a_func(int i)
{
  printf("a_func: %d\n\n", i);

  asm volatile ("li     3, 0xb00;"
                "mtvsrd 0, 3;"
		"addi   3, 3, 1;"
		"mtvsrd 1, 3;"
		"addi   3, 3, 1;"
		"li	4, 0xbee;"
		"mtvsrd 2, 4;"
		);
}

int main()
{
  a_func(0xf);
}
