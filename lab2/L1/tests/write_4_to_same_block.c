#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();
  /**
  Write 4 words into the same block, expecting 1 miss and 3 cache hits
  */
  value1 = 1;
  write(0, (uint8_t *)(&value1));

  value1 = 2;
  write(4, (uint8_t *)(&value1));

  value1 = 3;
  write(8, (uint8_t *)(&value1));

  value1 = 4;
  write(12, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;

  // will result in 1 miss and 3 hits 
  assert(total_time == DRAM_READ_TIME + 4*L1_WRITE_TIME && "Expected 1 write miss and 3 write hits");

  printf("Successful test!\n");

  return 0;
}
