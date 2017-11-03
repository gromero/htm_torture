#!/bin/bash
ulimit -c unlimited
gcc -O0 -g context_switch_.c -DMSR_FP -o context_switch_fp -pthread
touch core
while(true); do rm core; ./context_switch_fp &>/dev/null; gdb ./context_switch_fp ./core -q -ex 'print $vs0.uint128' -ex 'print $vs32.uint128'  -ex "quit"  |& egrep " = "; done
