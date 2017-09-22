#!/bin/bash -x
ulimit -c unlimited
gcc -O0 -g signal.c  -o signal -pthread
touch core
# while(true); do rm core; ./context_switch_trial &>/dev/null; gdb ./context_switch_trial ./core -q -ex 'print $vs0.uint128' -ex 'print $vs32.uint128'  -ex "quit"  |& egrep " = "; done
