#!/bin/bash

timeout 60 ./git/htm_torture/torture &>/dev/null
ERROR=$?

if [ $ERROR -eq 124 ]; then
echo "Timeout, kernel looks ok..."
exit 0
elif [ $ERROR -eq 132 ]; then
echo "Kernel is buggy..."
exit 1
else
echo "Unknown error: $ERROR. Check script!"
exit $ERROR
fi
