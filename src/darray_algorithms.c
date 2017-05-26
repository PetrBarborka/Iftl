#include "lcthw/darray_algorithms.h"
#include <bsd/stdlib.h>

int DArray_quicksort(DArray * array, DArray_compare cmp){
  qsort(array->contents, DArray_count(array), 
        sizeof(void*), cmp);
  return 0;
}
int DArray_heapsort(DArray * array, DArray_compare cmp){
  return heapsort(array->contents, DArray_count(array), 
                  sizeof(void*), cmp);
}
int DArray_mergesort(DArray * array, DArray_compare cmp){
  return mergesort(array->contents, DArray_count(array), 
                  sizeof(void*), cmp);
}

static void DArray_int_debug(DArray * array, char * name, int pivot){
  debug("Printing array %s", name);
  for ( int i = 0; i < DArray_count(array); i++ ){
    if ( i == pivot ){
      debug(" < [%d]: %d <", i, *(uint32_t *)DArray_get(array, i));
    } else {
      debug("[%d]: %d", i, *(uint32_t *)DArray_get(array, i));
    }
  }
}


int DArray_quicksort_custom(DArray * source, DArray_compare cmp){

  int region_start = 0;
  int region_end = source->end - 1;
  
  // used sort into
  DArray * temp = DArray_create(source->element_size, source->end);
  check_mem(temp);
  // borders of unsorted regions
  DArray * borders = DArray_create(sizeof(int), source->end+2);
  check_mem(borders);
  check(DArray_push_int(borders, region_start) == 0, 
        "Failed to push borders start");
  check(DArray_push_int(borders, region_end) == 0, 
        "Failed to push borders end");

  while(borders->end > 0){

    //DArray_int_debug(borders, "borders", -1);

    region_end = DArray_pop_int(borders);
    region_start = DArray_pop_int(borders);

    if ( region_end - region_start > 0 ){
      //1) Pick an element, called a pivot, from the array.
      void * pivot = DArray_get(source, region_start);
      check(pivot != NULL, "Failed to get pivot");
    //2) Partitioning: reorder the array so that all elements with values less than the pivot come before the pivot, while all elements with values greater than the pivot come after it (equal values can go either way). After this partitioning, the pivot is in its final position. This is called the partition operation.
      int i = region_start; //region_start is pivot
      for( int ctr = region_start + 1; ctr <= region_end; ctr++){
        void * el = DArray_get(source, ctr);
        check(el != NULL, "Failed to get element to reorder");
        if( cmp(&el, &pivot) <= 0 ){
          DArray_set(temp, i, el); 
          i++;
        }
      }

      //i is now at the pivot index
      DArray_set(temp, i, pivot);
      const int pivot_position = i;
      i++;

      for( int ctr = region_start + 1; ctr <= region_end; ctr++){
        void * el = DArray_get(source, ctr);
        check(el != NULL, "Failed to get element to reorder");
        if( cmp(&el, &pivot) > 0 ){
          DArray_set(temp, i, el); 
          i++;
        }
      }
      check(i == region_end + 1, 
            "Failed to assign all source region elements i: %d != re: %d",
            i, region_end);
            
      //DArray_int_debug(source, "source", -1);
      //DArray_int_debug(temp, "temp", pivot_position);

      // copy the partitioned stuff back
      for( int k = region_start; k <= region_end; k++ ){
        DArray_set(source, k, DArray_get(temp,k));
      }
      if( pivot_position - region_start > 1 ){
        check(DArray_push_int(borders, region_start) == 0, 
              "Failed to push borders start");
        check(DArray_push_int(borders, pivot_position - 1) == 0, 
              "Failed to push borders end");
      }
      if( region_end - pivot_position > 1 ){
        check(DArray_push_int(borders, pivot_position + 1) == 0, 
              "Failed to push borders start");
        check(DArray_push_int(borders, region_end) == 0, 
              "Failed to push borders end");
      }

    //3)Recursively apply the above steps to the sub-array of elements with smaller values and separately to the sub-array of elements with greater values.
    }
  }

  DArray_clear_destroy(borders);
  DArray_destroy(temp);
  return 0;
error:
  return -1;
}
static inline uint64_t nextpow2(uint64_t val){
  val -= 1;
  val |= val >> 1;
  val |= val >> 2;
  val |= val >> 4;
  val |= val >> 8;
  val |= val >> 16;
  val |= val >> 32;
  val += 1;
  //val >>= 1;
  return val;
}

static inline void DArray_swap(DArray * array, int i1, int i2){
  void * temp = DArray_get(array, i1);
  DArray_set(array, i1, DArray_get(array, i2));
  DArray_set(array, i2, temp);
}

static inline void DArray_sift_down(DArray * array, int index, int heap_end,
                                    DArray_compare cmp){
  int root = index;
  int left_child = root * 2 + 1;
  int to_swap = root;

  while ( left_child < heap_end ){

    int right_child = root * 2 + 2;

    void * r_el = DArray_get(array, root);
    void * lc_el = DArray_get(array, left_child);
    void * rc_el = DArray_get(array, right_child);


    if ( cmp(&r_el, &lc_el) < 0 ){
      to_swap = left_child;
      if ( right_child < heap_end && cmp(&lc_el, &rc_el) < 0 ){
        to_swap = right_child;
      }
    } else {
      if ( right_child < heap_end && cmp(&r_el, &rc_el) < 0 ){
        to_swap = right_child;
      }
    }

    
    if ( to_swap == root ) {
      return;
    } else {

      DArray_swap(array, root, to_swap);
      root = to_swap;
      left_child = root * 2 + 1;

    }
  } //while
} //fcn

static inline void DArray_heapify(DArray * array, DArray_compare cmp){
  int end = array->end;
  int start = (end - ( end % 2 == 0 ) - 1)/2 ;  

  while (start >= 0){
    DArray_sift_down(array, start, end, cmp);
    start--;
  }
}

int DArray_heapsort_custom(DArray * array, DArray_compare cmp){
  // 1) hepify array
  DArray_heapify(array, cmp);
  int heap_end = array->end;
  while( heap_end > 1 ){
    // 2) swap the biggest and the end of heap
    DArray_swap(array, 0, heap_end - 1);
    // 3) reduce heap size by 1 and restore heap by sift down
    heap_end--;
    DArray_sift_down(array, 0, heap_end, cmp);
  }
  return 0;
}
int DArray_mergesort_custom(DArray * array, DArray_compare cmp){

  int source_len = array->end;
  // used to sort into
  void ** temp = calloc(source_len, sizeof(void *)); 
  check_mem(temp);

  for(int width = 2; width <= (int) nextpow2((uint64_t)array->end); 
      width *= 2){
    int temp_counter = 0;
    for(int w = 0; w < array->end; w += width ){

      void ** r1 = array->contents + w;
      check(r1 != NULL, "Failed to get r1");
      void ** r2 = NULL;
      void ** r2_start = NULL;
      void ** rend = NULL;

      if( w + width/2 < array->end ){
        r2 = array->contents + (w + width/2);
        check(r2 != NULL, "Failed to get regular r2");
      } else {
        r2 = array->contents + (array->end - 1);
        check(r2 != NULL, "Failed to get default r2");
      }
      r2_start = r2;
      check(r2 != NULL, "Failed to get r2");
      if ( (w + width) <= array->end ){
        rend = array->contents + (w + width);
        check(rend != NULL, "Failed to get regular rend");
      } else {
        rend = array->contents + array->end;
        check(rend != NULL, "Failed to get default rend");
      }
      check(rend != NULL, "Failed to get rend");

      for( int k = w; ( k < w + width && k < array->end ); k++){ 
        if( (*r2 == *rend && *r1 != *r2_start) || 
            (*r2 != *rend && *r1 != *r2_start && cmp( r1, r2 ) < 0) ){
          temp[temp_counter] = *r1;
          r1++;
        } else {
          temp[temp_counter] = *r2;
          r2++;
        }
        temp_counter++;
      }
    }
    void ** temp_contents = temp;
    temp = array->contents;
    array->contents = temp_contents;
  }
  free(temp);
  return 0;
error:
  return 1;
}
