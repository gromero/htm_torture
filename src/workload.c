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
