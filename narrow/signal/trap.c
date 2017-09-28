#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

int kill_myself = 0;

pthread_t t0_ping;
pthread_t t1_pong;

void *ping(void *not_used)
{

  uint64_t i;

  // Wait an amount of context switches so load_fp and load_vec overflows and
  // MSR_{FP|VEC|VSX} = 0
  for(i=0; i < 1024*1024*512; i++);

#ifdef MSR_FP
  // Set MSR_FP = 1 before entering in TM
  asm("fadd    10, 10, 10;");
#endif

#ifdef MSR_VEC
  // Set MSR_VEC = 1 before entering in TM
  asm("vaddcuw 10, 10, 10;");
#endif

  asm (
	"   tbegin.             ;" // begin HTM
	"   beq       1f        ;" // failure handler
	"   trap                ;" // get into singal handler
	"   tend.               ;" // end HTM
	"1: nop                 ;"
 : : :);

  printf("Done. Returned fine from signal handler.\n");

  // Tell pong() to stop and return
  pthread_kill(t1_pong, SIGUSR1);
  return NULL;
}


void *pong(void *not_used)
{
  while (!kill_myself)
    // Induce a bunch of context switches in ping() thread that is by now just
    // sitting idle in the for() idle loop
    sched_yield();

  return NULL;
}


void trap_signal_handler(int signo, siginfo_t *si, void *notused)
{
  // Just print anything and return. Since we got here in HTM, we must just
  // get the failure handler since the transaction is aborted. Never a illegal
  // instruction
  printf("I'm in the signal handler after a trap. Returning...\n");
}


void usr1_signal_handler(int signo, siginfo_t *si, void *notused)
{
  // On receiving a USR1 from ping(), just set kill_myself so pong() returns
  kill_myself = 1;
}


int main(int argc, char **argv)
{
  pthread_attr_t attr;
  cpu_set_t cpuset;

  struct sigaction trap_sa;

  trap_sa.sa_flags = SA_SIGINFO;
  trap_sa.sa_sigaction = trap_signal_handler;
  sigaction(SIGTRAP, &trap_sa, NULL);

  struct sigaction usr1_sa;

  usr1_sa.sa_flags = SA_SIGINFO;
  usr1_sa.sa_sigaction = usr1_signal_handler;
  sigaction(SIGUSR1, &usr1_sa, NULL);

  // Set only CPU 0 in the mask. Both thread will be bind to cpu 0
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);

  // Init pthread attribute
  pthread_attr_init(&attr);

  // Bind thread ping() and pong() both to CPU 0 to they ping-pong
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);

  pthread_create(&t0_ping, &attr /* bind to cpu 0 */, ping, NULL);
  pthread_create(&t1_pong, &attr /* bind to cpu 0 */, pong, NULL);

  pthread_join(t0_ping, NULL);
  pthread_join(t1_pong, NULL);
}
