//
// Created by Mike Yang on 2024/2/25.
//

#include "cache.h"
#include "linked_list.h"
#include "util.h"
#include "params.h"

uint32_t measure_set_access_time(uint64_t addr)
{
  uint32_t cycles;

  asm volatile("mov %1, %%r8\n\t"
               "lfence\n\t"
               "rdtsc\n\t"
               "mov %%eax, %%edi\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "mov (%%r8), %%r8\n\t"
               "lfence\n\t"
               "rdtsc\n\t"
               "sub %%edi, %%eax\n\t"
      : "=a"(cycles) /*output*/
      : "r"(addr)
      : "r8", "edi");

  return cycles;
}

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
  uint64_t end = rdtsc()+cycles;
  while (rdtsc()<end ) {
    // Busy wait
  }
}

/**
 * Copy the address of the buffer into the linked list randomly
 * */
void rand_mem_cpy(cache_set* head, void* mem) {
  cache_set* curr = head;
  int total_lines = L2_SETS * L2_WAYS;

  cache_set **arr = (cache_set**) malloc(total_lines * sizeof(cache_set*));
  for (int i = 0; i < total_lines; i++) {
    // Here the cache_set structs are used to store the *line* *address* and set number
    arr[i] = (cache_set*) malloc(sizeof(cache_set));
    arr[i]->lineAddr = (uint64_t)mem + i * L2_LINE_SIZE;
    arr[i]->setNum = i / L2_WAYS; // Set number for this line
  }

  // Shuffle the lines within each set
  for (int i = 0; i < L2_SETS; i++) {
    shuffle((cache_set**)(arr + i * L2_WAYS), L2_WAYS);
  }
  printf ("total lines = %ld \n",total_lines);

  int buf_size = 1 << 21; // 2MB
  // Link the nodes
  for (int i = 0; i < total_lines; i++) {

    if (i >= buf_size / L2_LINE_SIZE) {
      fprintf(stderr, "Error: Attempt to write beyond buffer bounds.\n");
      break;
    }

    uint64_t offset = i * L2_LINE_SIZE;
    if (offset >= buf_size) {
      fprintf(stderr, "Error: Offset exceeds buffer size.\n");
      break;
    }

    uint64_t *ptr = (uint64_t *)((char *)mem + offset);

    // Ensure arr[i] is valid
    if (arr[i] == NULL) {
      fprintf(stderr, "Error: arr[%d] is NULL.\n", i);
      break;
    }

    *ptr = arr[i]->lineAddr;

    if(i%L2_WAYS != 0){
      continue;
    }
    curr->lineAddr = arr[i]->lineAddr;
    curr->setNum = arr[i]->setNum;

    curr = curr->next;
  }

  // Free the temporary array elements and array
  for (int i = 0; i < total_lines; i++) {
    free(arr[i]);
  }
  free(arr);
}


void prime_cache(cache_set* head, void*buf) {

  measure_set_access_time(head->lineAddr);
}

void probe_cache(cache_set* head, void*buf) {
  head->timing = measure_set_access_time(head->lineAddr);
//  serialize();
}

/**
 * Create the cache linked list
 * */
 cache_set * setup_cache(int ways, int sets, void* mem) {
  cache_set * head = setup_linked_list( sets);
//  rand_mem_cpy(head, mem, ways * sets,sets );
  rand_mem_cpy(head, mem );
  return head;
 }

void scramble_and_clear_cache (cache_set* cache, int ways, int sets, void* mem) {

   cache_set * curr = cache;
    while (curr != NULL) {
      curr->lineAddr = 0;
      curr->setNum = 0;
      curr->start = 0;
      curr->end = 0;
      curr = curr->next;

    }
//  rand_mem_cpy(cache, mem, ways * sets, sets );
  rand_mem_cpy(cache, mem );
}

 /**
  * Free the linked list cache
  * */
  void free_cache(cache_set * cache) {
    free_linked_list(cache);
  }

