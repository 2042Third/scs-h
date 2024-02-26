//
// Created by Mike Yang on 2024/2/25.
//

#include "cache.h"
#include "linked_list.h"

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
 * Create the cache linked list
 * */
 cache_line * setup_cache(int ways, int sets) {
   return setup_linked_list(ways, sets);
 }

 /**
  * Free the linked list cache
  * */
  void free_cache(cache_line * cache) {
    free_linked_list(cache);
  }