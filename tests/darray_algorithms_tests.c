#include "../src/lcthw/darray_algorithms.h"
#include "bsd/stdlib.h"
#include "minunit.h"
#include "time.h"

typedef int(*sorter_t)(DArray *, DArray_compare);

int testcmp(const char ** a, const char ** b){
  return strcmp(*a, *b);
}

int testcmp_int(uint32_t ** a, uint32_t ** b){
  return **a == **b ? 0 : **a > **b ? 1 : -1;
}

DArray * create_random_ints(int count){

  DArray * result = DArray_create(0, count);
  for(int i=0; i < count; i++){
    uint32_t * val = malloc(sizeof(uint32_t));
    *val = (uint32_t)(rand() % UINT32_MAX);
    DArray_push(result, val);
  }
  return result;

}

DArray * create_words(char * words[], int words_len){
    
  DArray * result = DArray_create(0, words_len);
  for(int i=0; i< words_len; i++){
    char * w = malloc(sizeof(char *));
    w = words[i];
    DArray_push(result, (void *)w);
  }
  return result;
}

void print_words(DArray * words, char * label){
  debug("---------\n%s: count: %d\n", label, DArray_count(words));
  for(int i=0; i<DArray_count(words); i++){
    debug("words[%d]: %s\n", i, 
        (char *)DArray_get(words, i));
  }
}

void print_ints(DArray * ints, char * label){
  debug("---------\n%s: count: %d\n", label, DArray_count(ints));
  for(int i=0; i<DArray_count(ints); i++){
    debug("ints[%d]: %d\n", i, 
        *(uint32_t *)DArray_get(ints, i));
  }
}

int is_sorted(DArray* array, DArray_compare cmp){
  for(int i = 0; i < DArray_count(array) - 1; i++){
    void * el1 = DArray_get(array, i);
    void * el2 = DArray_get(array, i + 1);
    if(cmp(&el1, &el2) > 0){
      return 0;
    }
  }
  return 1;
}

int contains_all(DArray* array, char * words [], int words_len){
  for(int i = 0; i < DArray_count(array); i++){
    char contains = 0;
    char * el = DArray_get(array, i);
    for( int j = 0; j < words_len; j++){
      if(strcmp(el, words[j]) == 0){
        contains = 1;
      }
    }
    if (!contains) return 0;
  }
  return 1;
}

char * run_sort_test(sorter_t sorter, char * name){

  char* words_in[] = {"Bob", "Alice", "Dan", "Cyril", "Filip", 
                    "Evzen", "Honza", "Gabriela", "Ivan", "Jiri" };
  int len_words_in = 10;
  DArray * words = create_words(words_in, len_words_in);
  mu_assert(name != NULL, "Define a name for the test");
  debug("Testing sorter %s", name);
  mu_assert(words != NULL, "Failed to create words");
  mu_assert(!is_sorted(words, (DArray_compare)testcmp), "Words should not start sorted");

  print_words(words, "Before");
  int rc = sorter(words, (DArray_compare)testcmp);
  print_words(words, "After");
  mu_assert(rc == 0,"Sort failed.");
  mu_assert(is_sorted(words, (DArray_compare)testcmp), "Sorter did not sort array.");
  mu_assert(contains_all(words, words_in, len_words_in), 
                         "Sorter did not sort array.");
  
  DArray_destroy(words);

  return NULL;
}

char * run_ints_sort_test(sorter_t sorter, char * name){

  DArray * ints = create_random_ints(10000);
  mu_assert(name != NULL, "Define a name for the test");
  debug("Testing sorter %s", name);
  mu_assert(ints != NULL, "Failed to create ints");
  print_ints(ints, "Before");
  mu_assert(!is_sorted(ints, (DArray_compare)testcmp_int), 
            "Ints should not start sorted");

  int rc = sorter(ints, (DArray_compare)testcmp_int);
  print_ints(ints, "After");
  mu_assert(rc == 0,"Sort failed.");
  mu_assert(is_sorted(ints, (DArray_compare)testcmp_int), 
            "Sorter did not sort array.");
  
  DArray_destroy(ints);

  return NULL;
}

char * test_quicksort(){
  return run_sort_test(DArray_quicksort, "quicksort");
}

char * test_quicksort_custom(){
  return run_sort_test(DArray_quicksort_custom, 
                       "quicksort_custom");
}

char * test_mergesort(){
  return run_sort_test(DArray_mergesort, "mergesort");
}

char * test_mergesort_custom(){
  return run_sort_test(DArray_mergesort_custom, 
                       "mergesort_custom");
}

char * test_heapsort(){
  return run_sort_test(DArray_heapsort, "heapsort");
}

char * test_heapsort_custom(){
  return run_sort_test(DArray_heapsort_custom, "heapsort_custom");
}

char * test_quicksort_ints(){
  return run_ints_sort_test(DArray_quicksort, "quicksort");
}

char * test_quicksort_custom_ints(){
  return run_ints_sort_test(DArray_quicksort_custom, 
                       "quicksort_custom");
}

char * test_mergesort_ints(){
  return run_ints_sort_test(DArray_mergesort, "mergesort");
}

char * test_mergesort_custom_ints(){
  return run_ints_sort_test(DArray_mergesort_custom, 
                       "mergesort_custom");
}

char * test_heapsort_ints(){
  return run_ints_sort_test(DArray_heapsort, "heapsort");
}

char * test_heapsort_custom_ints(){
  return run_ints_sort_test(DArray_heapsort_custom, "heapsort_custom");
}

char * run_all_tests(){

  mu_suite_start();
  srand(time(NULL));

  mu_run_test(test_quicksort);
  mu_run_test(test_quicksort_custom); 
  mu_run_test(test_mergesort);
  mu_run_test(test_mergesort_custom);
  mu_run_test(test_heapsort);
  mu_run_test(test_heapsort_custom);

  mu_run_test(test_quicksort_ints);
  mu_run_test(test_quicksort_custom_ints); 
  mu_run_test(test_mergesort_ints);
  mu_run_test(test_mergesort_custom_ints);
  mu_run_test(test_heapsort_ints);
  mu_run_test(test_heapsort_custom_ints);

#define BENCHMARK
#ifdef BENCHMARK
  mu_benchmark_test(test_quicksort, 1000);
  mu_benchmark_test(test_quicksort_custom, 1000); 
  mu_benchmark_test(test_mergesort, 1000);
  mu_benchmark_test(test_mergesort_custom, 1000);
  mu_benchmark_test(test_heapsort, 1000);
  mu_benchmark_test(test_heapsort_custom, 1000);

  mu_benchmark_test(test_quicksort_ints, 1000);
  mu_benchmark_test(test_quicksort_custom_ints, 1000); 
  mu_benchmark_test(test_mergesort_ints, 1000);
  mu_benchmark_test(test_mergesort_custom_ints, 1000);
  mu_benchmark_test(test_heapsort_ints, 1000);
  mu_benchmark_test(test_heapsort_custom_ints, 1000);
#endif // BENCHMARK

  return NULL;
}

RUN_TESTS(run_all_tests);
