#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;
  // uint32_t value2;

  resetTime();
  initCache();

  long start_time = getTime();

  /**
  Write 2 words into different blocks, expecting 2 cache misses 
  */
  value1 = 1;
  read(0, (uint8_t *)(&value1));

  value1 = 2;
  read(64, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;
  // two cache misses on write
  assert(total_time == DRAM_READ_TIME*2 + L1_WRITE_TIME*2 && "Expected 2 write misses");

  printf("Successful test!\n");

  return 0;
}
