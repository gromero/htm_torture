#!/bin/bash
#################################
#### TO BE USED IN THE GUEST ####
#################################

# Remove all traces of bisected kernels from the guest.
find /lib/modules -maxdepth 1 | awk '/\-[0-9a-z]+\+/ {print $0}' | sudo xargs -n1 rm -fr
find /boot | awk '/\-[0-9a-z]+\+/ {print $0}' | sudo xargs -n1 rm
