/*
 * Copyright (c) 2017 Breno Leitao, Gustavo Romero, IBM Corp.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include<stdio.h>
#include<inttypes.h>
#include "torture.h"

unsigned long *array;
extern void QuickSort(unsigned long *a, int left, int right);

/* Abort due to syscall (write); */
void scall()
{
	printf("Workload 0\n");
}

/* Simple fibonnaci function */
int fib(int x)
{
	if (x == 2 || x == 1)
		return 1;

	return fib(x-1) + fib(x-2);
}

/* Run a fibonnaci of 40 inside transaction */
void fibonacci() {
	int sum __attribute__((unused));
	int n = 40;

	sum = fib(n);
}

/* single nop  inside a transaction */
void nop() {
	_ ("or 0,0,0 \n");
}

/* Calling tabort inside transaction */
void tabort() {
	_ ("tabort.  0\n");
}

/* Quick sort workload */
void quicksort(int *A, int len)
{
	int pivot = A[len / 2];
	int i, j;
	int temp;

	if (len < 2) return;

	for (i = 0, j = len - 1; ; i++, j--) {
		while (A[i] < pivot) i++;
		while (A[j] > pivot) j--;

		if (i >= j) break;

		temp = A[i];
		A[i]     = A[j];
		A[j]     = temp;
	}

	quicksort(A, i);
	quicksort(A + i, len - i);
}

/* Quicksort workload */
void ownqsort()
{
	int a[] = {12, 3}; //, 9, 4, 9, 64};
	int n = sizeof a / sizeof a[0];
	quicksort(a, n);
}

/* loop */
void loop()
{
	int i ;
	for (i=0; i < 10; )
		i++;
}

// Infinite loop.
void iloop()
{
	while(1);
}

/*** workdload7 ***/
void utpsm_qsort()
{
	QuickSort(array, 0, ARRAY_SIZE-1);
}

// illegal instruction.
void illegal() {
	_ (".long 0x0");
}

// trap
void trap()
{
	_ ("trap");
}

void dscr()
{
	_ ("mfdscr 9");
	_ ("mtdscr 9");
}
