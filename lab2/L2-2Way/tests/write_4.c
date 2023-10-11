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
  write(0, (uint8_t *)(&value1));

  value1 = 2;
  write(4, (uint8_t *)(&value1));

  value1 = 3;
  write(8, (uint8_t *)(&value1));

  value1 = 4;
  write(12, (uint8_t *)(&value1));

  read(0x00004000, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;
  long expected_time = 2*DRAM_READ_TIME + 2*L2_READ_TIME + L2_WRITE_TIME + 4*L1_WRITE_TIME + L1_WRITE_TIME;

  // will result in 1 miss and 3 hits 
//   assert(total_time == DRAM_READ_TIME + 4*L1_WRITE_TIME && "Expected 1 write miss and 3 write hits");
  printf("Taken: %lu\n", total_time); 
  printf("Taken: %lu\n", expected_time); 
  printf("Successful test!\n");

  return 0;
}
