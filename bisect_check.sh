#!/bin/bash

CONFIG=/home/gromero/config-kernel
KERNEL_HOME=/home/gromero/git/linux
PATCH_HOME=/home/gromero/kernel/patches
BOOT=/boot
TORTURE_HOME=/home/gromero/git/htm_torture
TORTURE_CHECK=${TORTURE_HOME}/check.sh
GUEST=gromero16

function restart_vm {
  GUEST_VM_NAME=`virsh list --all | awk '/gromero16/ {print $2}'`
  echo "Restarting VM $GUEST_VM_NAME..."
  virsh destroy $GUEST_VM_NAME
  virsh start $GUEST_VM_NAME
}

echo -n "* Finding commit hash... "
COMMIT=`git log HEAD --oneline  -1 --pretty="%h"`
echo "$COMMIT"

echo "* Configuring new kernel on guest..."
ssh -o ServerAliveInterval=1 -o ServerAliveCountMax=1 $GUEST "
  sudo ppc64_cpu --smt=8 # turn on SMT
  cd $KERNEL_HOME
  git clean -xfd
  git reset --hard
  git checkout $COMMIT
  cp $CONFIG $KERNEL_HOME/.config
  # apply patches, if any...
  for patch in \`find /home/gromero/kernel/patches -type f\`; do patch -p1 < \$patch; done 
  make localmodconfig
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
  echo vmlinux=\$VMLINUX initrd=\$INITRD
# sudo grub-reboot "Advanced options for Ubuntu>Ubuntu, with Linux \${VMLINUX/\\/boot\\/vmlinux-/}"
# sudo update-grub
# reboot

  /boot/set_kernel.sh $COMMIT

# sudo kexec -x -l \$VMLINUX --initrd=\$INITRD --append=root=/dev/vda2
# sudo kexec -e
"

# Wait kernel boot and VM get back.
sleep 30

# Check bug.
echo "* Disabling SMT..."
timeout 60 ssh $GUEST sudo ppc64_cpu --smt=off
if [ $? -eq 124 ]; then
  echo "* Connection to VM failed after reboot, marking that kernel as bad..."
  restart_vm $GUEST
  exit 1 # exit as bad 
fi

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
