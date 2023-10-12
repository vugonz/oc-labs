
#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();

  read(0, (uint8_t *)(&value1));

  read(0x00004000, (uint8_t *)(&value1));

  read(0, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;
  long expected_time = 3*DRAM_READ_TIME + 3*L1_READ_TIME;

  // will result in 1 miss and 3 hits 
  assert(total_time == expected_time && "Expected 3 L1 misses");
  printf("Successful test!\n");
}