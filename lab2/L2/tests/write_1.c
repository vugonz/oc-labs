#include "../../Cache.h"
#include "../L2Cache.h"
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
  read(0, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;

  // will result in 1 miss and 3 hits 
  assert(total_time == DRAM_READ_TIME + L1_READ_TIME + L2_READ_TIME && "Expected 1 write miss and 3 write hits");

  printf("Taken: %lu\n", total_time);
  printf("Successful test!\n");

  return 0;
}
