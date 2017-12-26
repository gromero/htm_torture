#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <htmintrin.h>
#include <inttypes.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>

#define MSR_LE 1UL
#define LE     1UL


pthread_t t0_ping;
pthread_t t1_pong;

int exit_from_pong = 0;

int trap_event = 0;
int le = 0;

bool success;

void trap_signal_handler(int signo, siginfo_t *si, void *uc)
{
	ucontext_t *ucp = uc;
	uint64_t thread_endianness;

	/* Get thread endianness: extract bit LE from MSR */
	thread_endianness = MSR_LE & ucp->uc_mcontext.gp_regs[PT_MSR];

	/***
	 * Little-Endian Machine
	 */

	if (le) {
		/* First trap event */
		if (trap_event == 0) {
		/* Do nothing. Since it is returning from this trap event that
		 * endianness is flipped by the bug, we just let the process
		 * return from the signal handler to check on the second trap
		 * event if endianness is flipped or not.
		 */
		}
		/* Second trap event */
		else if (trap_event == 1) {
		/* If (a) endianness was still LE and we got a trap in TM on
		 * event 0, instruction (1) will be executed on return (rollback
		 * to tbegin. + 4). As (1) does nothing in effect, instruction
		 * (2) is executed again and so a 'trap' is executed (but not in
		 * TM). On the other hand, if (b) thread endianness is BE, i.e.
		 * endianness flipped, instruction (1) is treated as a b .+4 so
		 * instructions (3) and (4) are executed (as 'tbegin.' and
		 * 'trap') instead and we get again a trap (in TM mode). Either
		 * way (a or b) we can now check the MSR LE bit in the signal
		 * handler to see if endianness was flipped (bug) or not on the
		 * first trap. Getting a trap in TM mode or not is just worth
		 * noting because after checking for the endianness we
		 * manipulate NIP to return a success or fail: NIP on trap + TM
		 * is the address of tbegin. + 4 whilst NIP on trap in TM is
		 * simply the address of 'trap' instruction.
		 */
			if (thread_endianness == LE) {
				/* Go to 'success', i.e. instruction (6) */
				ucp->uc_mcontext.gp_regs[PT_NIP] += 16;
			} else {
				/* Thread endianness is BE, so it flipped
				 * inadvertently. Thus we flip back to LE and
				 * set IP to go to 'fail', i.e. instruction (5).
				 */
				ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL;
				ucp->uc_mcontext.gp_regs[PT_NIP] += 4;
			}
		}
	}

	/***
	 * Big-Endian Machine
	 */

	else {
		/* First trap event */
		if (trap_event == 0) {
			/* Force thread endianness to be LE */
			ucp->uc_mcontext.gp_regs[PT_MSR] |= 1UL;
		}
		/* Second trap event */
		else if (trap_event == 1) {
		/* Do nothing. If the bug is present on return from this trap
		 * event endianness will flip back "automatically" to BE,
		 * otherwise thread endianness will continue to be LE, as we set
		 * on the first trap event above.
		 */
		/* A third trap event */
		} else {
		/* If we reached out here it's because we got a third trap
		 * event in TM, meaning that instruction (4) was executed as
		 * 'trap', hence thread endianness is still LE and so no bug
		 * flipped back to BE on return from second trap event.
		 * Otherwise, bug flipped back to BE on return from the second
		 * trap event and so instruction (4) was executed as
		 * 'tdi 0, 0,-8065' (so basically a skip) and branch to 'fail'
		 * is taken after instruction (5) to indicate failure and we
		 * never reach out here.
		 */
			/* Flip back to BE and go to instruction (6), i.e. go to
			 * 'success'.
			 */
			ucp->uc_mcontext.gp_regs[PT_MSR] &= ~1UL;
			ucp->uc_mcontext.gp_regs[PT_NIP] += 8;
		}
	}
	/* Keep track of trap events */
	trap_event++;
}

void usr1_signal_handler(int signo, siginfo_t *si, void *not_used)
{
		/* Got a USR1 signal from ping(), so just tell pong() to exit */
		exit_from_pong = 1;
}

void *ping(void *not_used)
{
	uint64_t i;

	sleep(1);

	/* Wait an amount of context switches so load_fp and load_vec overflows
	 * and MSR_{FP|VEC|VSX} == 0.
	 */
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
        "   tdi  0, 0, 0x48     ;" // (1) 'tdi' (skip) or 'b .+8'
        "   trap                ;" // (2) trap
        "  .long 0x1D05007C     ;" // (3) tbegin. in LE
        "  .long 0x0800e07f     ;" // (4) 'trap' in LE, 'tdi' (skip) in BE
        "   b %l[fail]          ;" // (5) MSR LE flipped inadvertently
        "   b %l[success]       ;" // (6) MSR LE did no flip inadventently

	: : : : fail, success);

fail:
	printf("yes :-(\n");
	success = false;
	goto exit_from_ping;

success:
	printf("no! :-)\n");
	success = true;

exit_from_ping:
	/* Signs pong() to exit before leaving */
	pthread_kill(t1_pong, SIGUSR1);
	return NULL;
}


void *pong(void *not_used)
{
	sleep(1);

	while (!exit_from_pong)
		/* Induce context switches in ping() thread
		* until ping() finishes its job and signs
		* to exit from this loop.
		*/
		sched_yield();

  return NULL;
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

	/* Set only CPU 0 in the mask. Both threads will be bound to cpu 0. */
	CPU_ZERO(&cpuset);
	CPU_SET(0, &cpuset);

	/* Init pthread attribute */
	pthread_attr_init(&attr);

	/* Bind thread ping() and pong() both to CPU 0 so they ping-pong and
	 * fasten context switches on ping() thread, speeding up load_fp and
	 * load_vec overflow.
	 */
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpuset);


	/* Figure out the machine endianness */
	le = (int) *(uint8_t *)&k;

	if (le)
		printf("Little-Endian machine detected. Testing if endianness"
			" flips inadvertently... ");
	else
		printf("Big-Endian machine detected. Testing if endianness flips"
			" inadvertently... ");

	fflush(0);

	pthread_create(&t0_ping, &attr /* bind to cpu 0 */, ping, NULL);
	pthread_create(&t1_pong, &attr /* bind to cpu 0 */, pong, NULL);

	/* TODO: remove when adding to selftests */
	pthread_setname_np(t0_ping, "ping");
	pthread_setname_np(t1_pong, "pong");

	pthread_join(t0_ping, NULL);
	pthread_join(t1_pong, NULL);

	if (success)
		printf("result: Success\n");
	else
		printf("result: Failure!\n");
}
