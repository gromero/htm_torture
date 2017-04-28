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

/*
int utpsm_qsort()
{
        int size = arrSize;
        int left = 0, right = size-1;

        QuickSort(arrBase, left, right);

        Qsort_status = 0xAB1E0000;
        return 0;
}
*/


int main()
{
 array = (unsigned long *) calloc(ARRAY_SIZE, sizeof(unsigned long));

 // Fill array with pseudo-random values.
 for (int i = 0; i < ARRAY_SIZE; i++)
   array[i] = rand();

 // Print array.
 for (int i = 0; i < ARRAY_SIZE; i++)
   printf("%ld\n", array[i]);

 printf("===========================================================\n");

 // Order array.
 QuickSort(array,0, ARRAY_SIZE-1);

 // Print ordered array.
 for (int i = 0; i < ARRAY_SIZE; i++)
   printf("%ld\n", array[i]);

 exit(0);
}
