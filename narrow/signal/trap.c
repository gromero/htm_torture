#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

#define LE 1UL // MSR LE == 1 => LE

int kill_myself = 0;

pthread_t t0_ping;
pthread_t t1_pong;

int trap_event = 0;
int le = 0;

void *ping(void *not_used)
{

  uint64_t i;

  sleep(1);

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

  asm goto (
        "   tbegin.             ;" // (0) tbegin.
        "   tdi  0, 0, 0x48     ;" // (1) if (BE) skip; else b .+8
        "   trap                ;" // (2) go to signal handler to flip endianness (BE->LE)
        "  .long 0x1D05007C     ;" // (3) tbegin. LE, i.e. start a new transaction
        "  .long 0x0800e07f     ;" // (4) trap LE -> go again to sig. handler but do not change endianness, if (BE) -> nop
        "   b %l[fail]          ;" // (5) flipped back to BE but should not
        "   b %l[success]       ;" // (6) still LE -> success

 : : : : fail, success);

fail:
  printf("yes :-(\n");
  goto bail_out;

success:
  printf("no! :-)\n");

bail_out:
  // Tell pong() to stop and return
  pthread_kill(t1_pong, SIGUSR1);
  return NULL;
}


void *pong(void *not_used)
{
  sleep(1);

  while (!kill_myself)
    // Induce a bunch of context switches in ping() thread that is by now just
    // sitting idle in the for() idle loop
    sched_yield();

  return NULL;
}


void trap_signal_handler(int signo, siginfo_t *si, void *uc)
{

  ucontext_t *ucp = uc;
  uint64_t thread_endianness;

  thread_endianness = 1UL & ucp->uc_mcontext.gp_regs[PT_MSR];

//printf("Signal number: %d\n", signo);
//printf("%p\n", ucp->uc_mcontext.gp_regs[PT_NIP]);

  if (le) { // Little-Endian machine
    if (trap_event == 1) // It's the second trap event
      if (thread_endianness == LE) {
        ucp->uc_mcontext.gp_regs[PT_NIP] += 16;  // Goto 'success'
      } else { // Thread endianness is BE, so it flipped inadvertently
        ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL; // Flip back to LE
        ucp->uc_mcontext.gp_regs[PT_NIP] += 4;   // Goto 'fail'
      }

  } else {  // Big-Endian machine
    if (trap_event == 0) { // It's the first trap event
      // Force thread endianness flip to LE
      ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL;
    } else if (trap_event == 1) {
      // Do nothing. If the bug is present on return from this
      // trap event endiannes will flip back "automatically" to
      // BE, otherwise thread endianness will continue to be LE,
      // just as we set on first trap event above (trap_event == 0).
    } else { // trap event 2 (third trap event)
      // If we reached out here it's because we got a third trap
      // event, meaning that instruction X was executed as a 'trap'
      // instruction, hence thread endiannes is still LE, so the
      // bug did flip back to BE on return from trap event 1.
      ucp->uc_mcontext.gp_regs[PT_MSR] &= ~1UL; // Flip back to BE
      ucp->uc_mcontext.gp_regs[PT_NIP] += 8;    // Goto 'success'
    }
  }

  trap_event++;
}

void usr1_signal_handler(int signo, siginfo_t *si, void *not_used)
{
  // On receiving a USR1 from ping(), just set kill_myself so pong() returns
  kill_myself = 1;
}

int main(int argc, char **argv)
{
  uint16_t k = 1;

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

  // Set only CPU 0 in the mask. Both threads will be bound to cpu 0.
  CPU_ZERO(&cpuset);
  CPU_SET(0, &cpuset);

  // Init pthread attribute
  pthread_attr_init(&attr);

  // Bind thread ping() and pong() both to CPU 0 so they ping-pong
  // and fasten context switches on ping() thread, speeding up
  // load_fp and load_vec overflow.
  pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);


  // Detect machine endianness at run-time
  le = (int) *(uint8_t *)&k;

  if (le)
    printf("Little-Endian machine detected. Testing if endiannes flips inadvertently... ");
  else
    printf("Big-Endian machine detected. Testingif endiannes flips inadvertently... ");

  fflush(0);

  pthread_create(&t0_ping, &attr /* bind to cpu 0 */, ping, NULL);
  pthread_create(&t1_pong, &attr /* bind to cpu 0 */, pong, NULL);

  pthread_setname_np(t0_ping, "ping");
  pthread_setname_np(t1_pong, "pong");

  pthread_join(t0_ping, NULL);
  pthread_join(t1_pong, NULL);
}
