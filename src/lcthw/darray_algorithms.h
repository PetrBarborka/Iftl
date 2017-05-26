#ifndef __darray_algorithms_h__
#define __darray_algorithms_h__

#include "lcthw/darray.h"

typedef int(*DArray_compare)(const void* a, const void* b);

int DArray_quicksort(DArray * array, DArray_compare cmp);
int DArray_heapsort(DArray * array, DArray_compare cmp);
int DArray_mergesort(DArray * array, DArray_compare cmp);

int DArray_quicksort_custom(DArray * array, DArray_compare cmp);
int DArray_heapsort_custom(DArray * array, DArray_compare cmp);
int DArray_mergesort_custom(DArray * array, DArray_compare cmp);

#endif // __darray_algorithms_h__
