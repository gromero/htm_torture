#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <ucontext.h>
#include <pthread.h>
#include <signal.h>

/* Generates the following after some time running:

[ 4642.360317] Bad kernel stack pointer 7fff7fd7e710 at c00000000000bd7c
[ 4642.360406] Oops: Bad kernel stack pointer, sig: 6 [#13]
[ 4642.360412] SMP NR_CPUS=2048 
[ 4642.360412] NUMA 
[ 4642.360416] PowerNV
[ 4642.360422] Modules linked in: xt_CHECKSUM iptable_mangle ipt_MASQUERADE nf_nat_masquerade_ipv4 iptable_nat nf_nat_ipv4 nf_nat nf_conntrack_ipv4 nf_defrag_ipv4 xt_conntrack nf_conntrack ipt_REJECT nf_reject_ipv4 xt_tcpudp tun bridge stp llc kvm_hv kvm_pr kvm ebtable_filter ebtables ip6table_filter ip6_tables iptable_filter ip_tables x_tables fuse nfsd auth_rpcgss oid_registry nfs_acl nfs lockd grace fscache sunrpc binfmt_misc ext4 crc16 jbd2 mbcache sg powernv_rng rng_core autofs4 xfs libcrc32c crc32c_generic dm_mod sr_mod ses enclosure sd_mod cdrom scsi_transport_sas xhci_pci xhci_hcd usbcore tg3 ptp pps_core libphy ipr
[ 4642.360627] CPU: 32 PID: 46485 Comm: bad_kernel_stac Tainted: G      D         4.12.0-pristine #9
[ 4642.360706] task: c0000007d9abea00 task.stack: c0000007e4cac000
[ 4642.360763] NIP: c00000000000bd7c LR: 00007fff7ff4809c CTR: 00000000203b0a60
[ 4642.360830] REGS: c00000003fe7fd40 TRAP: 0700   Tainted: G      D          (4.12.0-pristine)
[ 4642.360908] MSR: 9000000102a03031 <SF,HV,VEC,VSX,FP,ME,IR,DR,LE,TM[E]>
[ 4642.360915]   CR: 02004482  XER: 00000000
[ 4642.361004] CFAR: c00000000000bcb4 SOFTE: 0 
               GPR00: 00007fff7ff4805c 00007fff7fd7e710 00007fff7ff77f00 00007fffe6aeacf8 
               GPR04: 0000000000000000 00007fff7fd7e950 00007fff7fd7f1a0 00007fff7ff48034 
               GPR08: 0000000000000000 00007fff7fd7e770 00007fff7fd7f1a0 0000000000000000 
               GPR12: 00000000203b0a60 00007fff7fd86900 0000000000000000 00007fff7f580000 
               GPR16: 00007fff7fd7f260 00007fff7fff0000 00007fffe6aeacf8 00007fffe6aeacf0 
               GPR20: 00007fff7ff74390 00000000203b0a60 0000000000000000 00007fff7ff743a0 
               GPR24: 0000000000010000 00007fffe6aeabf0 0000000000000003 0000000000800000 
               GPR28: 00007fffe6aeac28 00000000003d0f00 00007fff7fd86900 00007fff7fd7e710 
[ 4642.361579] NIP [c00000000000bd7c] fast_exception_return+0xac/0xb4
[ 4642.361637] LR [00007fff7ff4809c] 0x7fff7ff4809c
[ 4642.361682] Call Trace:
[ 4642.361705] Instruction dump:
[ 4642.361740] f84d03d8 e9a100d8 7c7b03a6 e84101a0 7c4ff120 e8410170 7c5a03a6 e8010070 
[ 4642.361811] e8410080 e8610088 e8810090 e8210078 <4c000024> 48000000 e8610178 88ed02bb 
[ 4642.361882] ---[ end trace 965fa69bff047160 ]---
*/

/*******************/
/* SIGNAL HANDLER  */
/*******************/

void advanced_signal_handler(int signo, siginfo_t *si, void *data)
{
 printf("* Received a SIGTRAP\n");
}

/***********/
/* WORKER  */
/***********/

void* worker_with_htm_and_trap(void *sig)
{
  asm (
       "   tbegin. \n\t"
       "   beq 2f  \n\t"
       "   trap    \n\t" // <==== REMOVE ME TO AVOID CRASH
       "   tend.   \n\t"
       "2:         \n\t"
       :
       :
       :
      );
}

int main(void)
{
 pthread_t t1;

 sigset_t sigset;

 struct sigaction sa;

 sa.sa_flags = SA_SIGINFO;
 sa.sa_sigaction = advanced_signal_handler;
 sigaction(SIGTRAP, &sa, NULL);
 sigaction(SIGILL , &sa, NULL);

 sigemptyset(&sigset);
 sigaddset(&sigset, SIGTRAP);

 pthread_sigmask(SIG_UNBLOCK, &sigset, NULL);

 // Execute worker
 pthread_create(&t1, NULL, worker_with_htm_and_trap, (void*) &sigset);

 pthread_join(t1, NULL);

 exit(0);
}
