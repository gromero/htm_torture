#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>

void sleeper (void);

pthread_t ping_tid;
pthread_t pong_tid;

void *
ping (void *not_used)
{

  uint64_t high;
  uint64_t low;	
  uint64_t texasr;	

  sleep (1);			

  sleeper ();

#ifdef MSR_VEC
  asm ("vaddcuw 10, 10, 10;");	
#endif

#ifdef MSR_FP
  asm ("fadd    10, 10, 10;");
#endif

_do_htm:
  asm ("1: tbegin.             ;"
	    "   beq       3f        ;"	
	    "   trap                ;"	
	    "   tend.               ;"	
	    "3:     nop             ;"
            :
	    :
            : 
	    );

}


void *
pong (void *not_used)
{
  pong_tid = pthread_self ();
  sleep (1);			
  while (1)
    {
      sched_yield ();
    }	
}


void
signal_handler (int signo, siginfo_t * si, void *notused)
{
  // We don't skip trap on purpose here
  printf ("Got a trap instruction in HTM...\n");
}

int
main (int argc, char **argv)
{
  pthread_t t0, t1;
  pthread_attr_t attr;
  cpu_set_t cpuset;

  struct sigaction sa;

  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = signal_handler;
  sigaction (SIGTRAP, &sa, NULL);

  // Set only CPU 0 in the mask. Both thread will be bind to cpu 0
  CPU_ZERO (&cpuset);
  CPU_SET (0, &cpuset);

  // init pthread attribute
  pthread_attr_init (&attr);

  // Set CPU 0 mask into the pthread attribute
  pthread_attr_setaffinity_np (&attr, sizeof (cpu_set_t), &cpuset);

  pthread_create (&t0, &attr /* bind to cpu 0 */ , ping, NULL);
  pthread_create (&t1, &attr /* bind to cpu 0 */ , pong, NULL);

  pthread_setname_np (t0, "ping");	// for systemtap probe install convenience
  pthread_setname_np (t1, "pong");	// for systemtap probe install convenience

  pthread_join (t0, NULL);
  pthread_join (t1, NULL);
}
