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
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 1024

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
                else if(*(a+(*PivotLoc)) < *(a+left))
                {
                        Swap((a+(*PivotLoc)), (a+left));
                        *PivotLoc = left;
                        right--;
                }
        } // end while(1)
}

void QuickSort(unsigned long *a, int left, int right)
{
        if(left < right)
        {
                int PivotLoc;
                PartitionArray(a, left, right, &PivotLoc);
                QuickSort(a, left, PivotLoc-1);
                QuickSort(a, PivotLoc+1, right);
        }
}
