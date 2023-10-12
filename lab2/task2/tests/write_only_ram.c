#include "../../Cache.h"
#include "../L2Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();

  /**
  Test that writes back a L2 block
  */
  value1 = 1;
  write(0x4000, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;
  long expected_time = 2*DRAM_READ_TIME + 2*L2_READ_TIME + 2*L1_READ_TIME +
                        + L1_WRITE_TIME + L2_WRITE_TIME;

  // will result in 1 miss and 3 hits 
//   assert(total_time == expected_time);
  // printf("Taken: %lu\n", total_time); 
  // printf("Expected: %lu\n", expected_time); 
  // printf("Successful test!\n");
}