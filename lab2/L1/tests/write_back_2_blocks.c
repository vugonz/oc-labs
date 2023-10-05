#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  resetTime();
  initCache();

  long start_time = getTime();
  // writting 1, 2, 3, 4, into every word in first block 
  for (int i = 0; i < 16; ++i) {
    write(i*4, (uint8_t *)(&i));
  }

  // doing the same thing for second block
  for (int i = 0; i < 16; ++i) {
    write(64 + i*4, (uint8_t *)(&i));
  }

  uint32_t value1 = 0;
  // force block1 write back
  read(0x00004000, (uint8_t *)(&value1));

  value1 = 0;
  // force block2 write back
  read(0x00004040, (uint8_t *)(&value1));

  long total_time = getTime() - start_time;

  long expected_time = DRAM_READ_TIME + 16*L1_WRITE_TIME +
                        + DRAM_READ_TIME + 16*L1_WRITE_TIME +
                        + DRAM_READ_TIME + DRAM_WRITE_TIME + L1_READ_TIME +
                        + DRAM_READ_TIME + DRAM_WRITE_TIME + L1_READ_TIME;

  assert(total_time == expected_time && "Expected time and actual time differ! Writting back 2 blocks");
  // will result in 1 miss and 3 hits 
  printf("Successful test!\n");

  return 0;
}
