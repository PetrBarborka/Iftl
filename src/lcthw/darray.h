#ifndef __darray_h__
#define __darray_h__

#include <stdlib.h>
#include <assert.h>
#include <lcthw/dbg.h>


// Dynamic array of void *
// Size allocated scales dynamically.
// Dynamic access: push and pop scales the array
//    automatically.
// Random acces: get and set allows access & 
//    manipulation of elements [0-max) with
//    bounds checking, no resizing. 
//    Setting or getting an element in range 
//    [end-max) pushes end to the provided index.
// The default value of an element is NULL 

typedef struct DArray {
  int max;
  int end;
  size_t element_size;
  size_t expand_rate;
  void ** contents;
} DArray;

DArray * DArray_create(size_t element_size, size_t initial_max);
void DArray_destroy(DArray * array);
void DArray_clear(DArray * array);
void DArray_clear_destroy(DArray * array);
int DArray_contract(DArray * array);
int DArray_expand(DArray * array);
int DArray_push(DArray * array, void * el);
void * DArray_pop(DArray * array);
int DArray_push_int(DArray * array, int to_push);
int DArray_pop_int(DArray * array);

#define DArray_first(A) ((A)->contents[0])
#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) ((A)->end)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline void DArray_set(DArray * array, int index, void * element){
  check(index >= 0, "Negative index into array");
  check(index < array->max, "Setting an element outside allocated space (> DArray_max(A))");
  array->contents[index] = element;
  if(index >= array->end){
    array->end = index + 1;
  }
error:
  return;
}

static inline void * DArray_get(DArray * array, int index){
  check(index >= 0, "Negative index into array");
  check(index < array->max, "Getting an element outside allocated space (> DArray_max(A))");
  if(index > array->end){
    array->end = index;
  }
  return array->contents[index];
error:
  return NULL;
}

static inline void * DArray_remove(DArray * array, int index){
  check(index >= 0, "Negative index into array");
  check(index < array->max, "Getting an element outside allocated space (> DArray_max(A))");

  void * el = array->contents[index];
  array->contents[index] = NULL;
  return el;
error:
  return NULL;
}

static inline void * DArray_new(DArray * array){
  check(array->element_size > 0, "Can't use DArray_new with element size 0");
  return calloc(1, array->element_size);
error:
  return NULL;
}

#define DArray_free(A) free((A))

#endif // __darray_h__
