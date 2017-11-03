#include "torture.h"

void signal_handler(int signo, siginfo_t *si, void *data)
{
 // Set a local pointer to uc.
 // ucontext_t *uc = (ucontext_t *)data;

 // printf("* Received a SIGTRAP\n");
 // printf("* si->si_addr = %p\n", si->si_addr);
 // printf("* uc->uc_mcontext.regs->nip: %p\n", (void *) uc->uc_mcontext.regs->nip);

 // uc->uc_mcontext.regs->nip += 4; // Skip illegal instruction.
 // uc->uc_mcontext.gp_regs[32] += 4; // Same as above ;-)
}
