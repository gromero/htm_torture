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

#include <stdio.h>
#include <stdlib.h>

unsigned long *array;

void Swap(unsigned long *a, unsigned long *b)
{
	int temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

void PartitionArray(unsigned long *a, int left, int right, int *PivotLoc)
{
	*PivotLoc = left;

	while(1)
	{
		while(*(a+(*PivotLoc)) <= *(a+right) && *PivotLoc != right)
		{
			right--;
		}
		if(*PivotLoc == right)
		{
			break;
		}
		else if(*(a+(*PivotLoc)) > *(a+right))
		{
			Swap((a+(*PivotLoc)), (a+right));
			*PivotLoc = right;
			left++;
		}

		while(*(a+(*PivotLoc)) >= *(a+left) && *PivotLoc != left)
		{
			left++;
		}
		if(*PivotLoc == left)
		{
			break;
		}
		else if(*(a+(*PivotLoc)) < *(a+left)) {
			Swap((a+(*PivotLoc)), (a+left));
			*PivotLoc = left;
			right--;
		}
	} // end while(1)
}

void QuickSort(unsigned long *a, int left, int right)
{
	if (left < right) {
		int PivotLoc;

		PartitionArray(a, left, right, &PivotLoc);
		QuickSort(a, left, PivotLoc-1);
		QuickSort(a, PivotLoc+1, right);
	}
}
