#!/bin/bash

CONFIG=/home/gromero/config-kernel
KERNEL_HOME=/home/gromero/git/linux
PATCH_HOME=/home/gromero/kernel/patches
BOOT=/boot
TORTURE_HOME=/home/gromero/git/htm_torture
TORTURE_CHECK=${TORTURE_HOME}/check.sh
GUEST=gromero16

echo -n "* Finding commit hash... "
COMMIT=`git log HEAD --oneline  -1 --pretty="%h"`
echo "$COMMIT"

echo "* Configuring new kernel on guest..."
ssh -o ServerAliveInterval=1 -o ServerAliveCountMax=4 $GUEST "
  sudo ppc64_cpu --smt=8 # turn on SMT
  cd $KERNEL_HOME
  git clean -xfd
  git checkout $COMMIT
  cp $CONFIG $KERNEL_HOME/.config
  # apply patches, if any...
  for patch in \`find /home/gromero/kernel/patches -type f\`; do patch -p1 < \$patch; done 
  make olddefconfig 
"

echo "* Adjusting CONFIG_LOCALVERSION=$COMMIT..."
#  # Set kernel CONFIG_LOCALVERSION equal to the commit hash.
ssh $GUEST sed -i "s\|CONFIG_LOCALVERSION=\\\"\\\"\|CONFIG_LOCALVERSION=\\\"-$COMMIT\\\"\|" $KERNEL_HOME/.config

echo "* Building and booting new kernel..."
ssh $GUEST -o ServerAliveInterval=1 -o ServerAliveCountMax=1 "
  cd $KERNEL_HOME
  make -j66 clean
  make -j66
  sudo make -j66 modules_install
  sudo make -j66 install
  # kexec:
  INITRD=\`find $BOOT -type f | fgrep $COMMIT | fgrep initrd\`
  VMLINUX=\`find $BOOT -type f | fgrep $COMMIT | fgrep vmlinux\`
  echo \$VMLINUX \$INITRD
  sudo kexec -x -l \$VMLINUX --initrd=\$INITRD --append=root=/dev/vda2
  sudo kexec -e
"

# Wait kernel boot and VM get back.
# sleep 2

# Check bug.
echo "* Disabling SMT..."
ssh $GUEST sudo ppc64_cpu --smt=off

echo "* Checking bug..."
ssh $GUEST $TORTURE_CHECK
RES=$?

if [ $RES -eq 0 ]; then
  # add clean routine here
  exit 0 # kernel is ok, so it's bad for us here!.
elif [ $RES -eq 1 ]; then
  # add clean routine here
  exit 1 # kernel is buggy, so it's good for us here!.
fi
