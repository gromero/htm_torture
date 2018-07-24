/*
 * Copyright (c) 2017 Breno Leitao, Gustavo Romero, IBM Corp.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

#include "torture.h"

#define DEBUG
void signal_handler(int signo, siginfo_t *si, void *data)
{
#ifdef DEBUG_SIGNAL
	// Set a local pointer to uc.
	ucontext_t *uc = (ucontext_t *)data;

	printf("* Received a SIGTRAP\n");
	printf("* si->si_addr = %p\n", si->si_addr);
	printf("* uc->uc_mcontext.regs->nip: %p\n",
		(void *) uc->uc_mcontext.regs->nip);

	uc->uc_mcontext.regs->nip += 4; // Skip illegal instruction.
	uc->uc_mcontext.gp_regs[32] += 4; // Same as above ;-)
#endif
#ifdef DEBUG
	printf(".");
#endif
}
