//
// Created by Mike Yang on 2024/2/25.
//

#include "cache.h"
#include "linked_list.h"
#include "util.h"

// Function to use cpuid for serialization
void serialize() {
  uint32_t eax, edx;
  cpuid(0, &eax, &edx); // Using CPUID with EAX=0 for serialization
}

/**
 * Busy wait for an approximate number of cycles.
 * invariant: the number of cycles spent in this function is the least amount of cycles it should wait
 * */
void busy_wait_cycles(uint64_t cycles) {
  uint64_t start = rdtsc();
  while ((rdtsc() - start) < cycles) {
    // Busy wait
  }
}

/**
 * Copy the address of the buffer into the linked list randomly
 * */
void rand_mem_cpy(cache_line* head, void* mem, size_t size, size_t ways) {
  cache_line* curr = head;
  char ** arr = (char **) malloc(size * sizeof(set_line_addr*));
  for (int i = 0; i < size; i++) { // copy the address of the buffer into array sequentially
    set_line_addr* cache = (set_line_addr*) malloc(sizeof(set_line_addr));
    cache->lineAddr = (uint64_t) mem+i;
    cache->setNum = i % ways;
    arr[i] = (char*) cache;
  }
  shuffle((char **) arr, size);
  for (int i = 0; i < size; i++) {
    set_line_addr* cache = (set_line_addr*) arr[i];
    curr->lineAddr = cache->lineAddr;
    curr->setNum = cache->setNum;
    curr = curr->next;
  }
  free(arr);
}
cache_line* recur_prime_cache(cache_line* cache){
  serialize();
//  cache->start = rdtsc();
  (*((char *)cache->lineAddr)) ++;
  mfence();
//  cache->end = rdtscp();
  serialize();
  return cache->next;
}

void prime_cache(cache_line* head) {
  cache_line* curr = head;
  while (curr != NULL){
    curr = recur_prime_cache(curr);
  }
}

/**
 * Create the cache linked list
 * */
 cache_line * setup_cache(int ways, int sets, void* mem) {
   cache_line * head = setup_linked_list(ways, sets);

   rand_mem_cpy(head, mem, ways * sets,ways );
   return head;
 }

 /**
  * Free the linked list cache
  * */
  void free_cache(cache_line * cache) {
    free_linked_list(cache);
  }

