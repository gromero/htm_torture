#!/bin/bash -x
ulimit -c unlimited
make
./signal
