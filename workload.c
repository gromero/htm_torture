#include<stdio.h>
#include<inttypes.h>
#include "x.h"

int fib(int x){
	if (x == 2 || x == 1)
		return 1;

	return fib(x-1) + fib(x-2);
}

/* Abort due to syscall (printf); */
void workload0() {
	printf("Workload 0\n");
}

/* Run a fibonnaci of 40 inside transaction */
void workload1() {
	int sum;
	int n = 40;

	sum = fib(n);
	printf("Workload 1\n");
}

/* single nop  inside a transaction */
void workload2() {
	_ ("or 0,0,0 \n");
}

/* Calling tabort inside transaction */
void workload3() {
	_ ("tabort.  0\n");
}

/* Quick sort workload */
void quicksort(int *A, int len)
{
  if (len < 2) return;
 
  int pivot = A[len / 2];
 
  int i, j;
  for (i = 0, j = len - 1; ; i++, j--)
  {
    while (A[i] < pivot) i++;
    while (A[j] > pivot) j--;
 
    if (i >= j) break;
 
    int temp = A[i];
    A[i]     = A[j];
    A[j]     = temp;
  }
 
  quicksort(A, i);
  quicksort(A + i, len - i);
}

void workload4() {
	int a[] = {12, 3, 9, 4, 9, 64};
	int n = sizeof a / sizeof a[0];
	quicksort(a, n);
}

/* abort due to illegal instruction */
void workload5() {
// Causing a illegal instruction and causing a segfault
//	_ (".long 0x0	\n");
}
