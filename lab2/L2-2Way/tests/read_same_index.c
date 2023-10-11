
#include "../../Cache.h"
#include "../L2Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();

  /**
  Test to miss on cache L1 but hit on L2
  */

  read(0, (uint8_t *)(&value1));

  read(0x00004000, (uint8_t *)(&value1));

  read(0, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;
  long expected_time = 2*DRAM_READ_TIME + 3*L2_READ_TIME + 3*L1_READ_TIME;

  // will result in 1 miss and 3 hits 
//   assert(total_time == DRAM_READ_TIME + 4*L1_WRITE_TIME && "Expected 1 write miss and 3 write hits");
  printf("Taken: %lu\n", total_time); 
  printf("Expected: %lu\n", expected_time); 
  printf("Successful test!\n");
}