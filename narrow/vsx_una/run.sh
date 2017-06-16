#!/bin/bash

# enable core file generation
ulimit -c unlimited

# test state after VSX unavailable in HTM with MSR.[FP= 0 VEC=0 VSX=0]
gcc -O0 -g vsx_una_.c -o vsx_una_ -pthread
./vsx_una_
echo "MSR.[FP=0 VEC=0 VSX=0]"
gdb ./vsx_una_ ./core -q -ex "info r vs0" -ex "info r vs32" -ex "quit" 2>/dev/null  | egrep "vs0|vs32"
echo ""

# test state after VSX unavailable in HTM with MSR.[FP=1 VEC=0 VSX=0]
gcc -O0 -g -DMSR_FP vsx_una_.c -o vsx_una_fp -pthread
./vsx_una_fp
echo "MSR.[FP=1 VEC=0 VSX=0]"
gdb ./vsx_una_fp ./core -q -ex "info r vs0" -ex "info r vs32" -ex "quit" 2>/dev/null  | egrep "vs0|vs32"
echo ""

# test state after VSX unavailable in HTM with MSR.[FP=0 VEC=1 VSX=0]
gcc -O0 -g -DMSR_VEC vsx_una_.c -o vsx_una_vec -pthread
./vsx_una_vec
echo "MSR.[FP=0 VEC=1 VSX=0]"
gdb ./vsx_una_vec ./core -q -ex "info r vs0" -ex "info r vs32" -ex "quit" 2>/dev/null  | egrep "vs0|vs32"
echo ""

# test state after VSX unavailable in HTM with MSR.[FP=1 VEC=1 VSX=0]
gcc -O0 -g -DMSR_FP -DMSR_VEC vsx_una_.c -o vsx_una_fp_vec -pthread
./vsx_una_fp_vec
echo "MSR.[FP=1 VEC=1 VSX=0]"
gdb ./vsx_una_ ./core -q -ex "info r vs0" -ex "info r vs32" -ex "quit" 2>/dev/null  | egrep "vs0|vs32"
echo ""
