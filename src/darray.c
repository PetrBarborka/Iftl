#include "lcthw/darray.h"
#include "assert.h"

DArray * DArray_create(size_t element_size, size_t initial_max){

  DArray * array = malloc(sizeof(DArray));
  check_mem(array);
  array->max = initial_max;
  check(array->max > 0,"You must set initial max > 0.");

  array->contents = calloc(initial_max, sizeof(void*));
  check_mem(array->contents);

  array->end = 0;
  array->element_size = element_size;
  array->expand_rate = DEFAULT_EXPAND_RATE;

  return array;
error:
  if (array) free(array);
  return NULL;
}

void DArray_destroy(DArray * array){
  if (array != NULL) {
    if (array->contents != NULL){
      free(array->contents);
      array->contents = NULL;
    }
    free(array);
    array = NULL;
  }
}

void DArray_clear(DArray * array){
  int i = 0;
  if (array->end > 0){
    for(;i<array->max;i++){
      if (array->contents[i] != NULL){
        free(array->contents[i]);
        array->contents[i] = NULL;
      }
    }
  }
}

void DArray_clear_destroy(DArray * array){
  DArray_clear(array);
  DArray_destroy(array);
}

static inline int DArray_resize(DArray * array, size_t new_size){
  
  array->max = new_size;
  check(array->max > 0, "New size must be > 0");

  void * contents = realloc(array->contents,
                            array->max * sizeof(void *));

  check_mem(contents);
  array->contents = contents;
  return 0;

error:
  return -1;
}

int DArray_contract(DArray * array){
  int new_size = array->end < (int)array->expand_rate ?
                 (int)array->expand_rate : array->end;

  return DArray_resize(array, new_size + 1);
}

int DArray_expand(DArray * array){
  int old_size = array->max;
  int new_size = array->max + array->expand_rate;
  check(DArray_resize(array, new_size) == 0,
        "Failed to resize array from %d to %d",
        old_size, new_size);
  
  memset(array->contents + old_size, 0, array->expand_rate + 1);
  return 0;

error:
  return 1;
}
int DArray_push(DArray * array, void * el){

  array->contents[array->end] = el;
  array->end++;

  if(array->end >= array->max){
    return DArray_expand(array);
  } else {
    return 0;
  }
}

void * DArray_pop(DArray * array){
  check(array->end >= 1, "Can't pop from an empty array");

  void * val = DArray_remove(array, array->end - 1);
  array->end--;

  if(array->end > (int)array->expand_rate &&
      array->end % (int)array->expand_rate){
    DArray_contract(array);
  }

  return val;

error:
  return NULL;
}

int DArray_push_int(DArray * array, int to_push){
  check(array->element_size = sizeof(int), 
        "Pushing int into a non-int DArray");
  int * p = malloc(sizeof(int));
  *p = to_push;
  return DArray_push(array, (void *)p);
error:
  return 0;
}

int DArray_pop_int(DArray * array){
  check(array->element_size = sizeof(int), 
        "Pushing int into a non-int DArray");
  int * op = ((int *)DArray_pop(array));
  int out = *op;
  free(op); op = NULL;
  return out;
error:
  return 0;
}
