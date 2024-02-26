//
// Created by Mike Yang on 2024/2/25.
//

#ifndef LAB1_15793_CACHE_H
#define LAB1_15793_CACHE_H

#include <stdlib.h>


void serialize();
void busy_wait_cycles(uint64_t cycles);

static inline uint64_t rdtsc()__attribute__((always_inline));
static inline uint64_t rdtscp()__attribute__((always_inline));
static inline void mfence()__attribute__((always_inline));
static inline void cpuid(int code, uint32_t* a, uint32_t* d)__attribute__((always_inline));

// Read the Time Stamp Counter
static inline uint64_t rdtsc() {
  uint32_t lo, hi;
  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

// Read the Time Stamp Counter and the Processor ID
static inline uint64_t rdtscp() {
  uint32_t lo, hi;
  asm volatile("rdtscp" : "=a"(lo), "=d"(hi) :: "%rcx");
  return ((uint64_t)hi << 32) | lo;
}

// Memory fence, Miro Haller, June 2020
static inline void mfence() {
  asm volatile(
      "mfence\n\t"
      ::
      );
}

// for serialization
static inline void cpuid(int code, uint32_t* a, uint32_t* d) {
  asm volatile("cpuid"
      : "=a"(*a), "=d"(*d) // output
      : "0"(code)           // input
      : "ebx", "ecx");      // clobbered register
}



#endif //LAB1_15793_CACHE_H
