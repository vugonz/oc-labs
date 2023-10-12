#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>
#include <stdint.h>

int main() {
  uint32_t value1;
  // uint32_t value2;

  resetTime();
  initCache();

  long start_time = getTime();

  /**
  Write to all words in a block, then, write block back in memory and finally
  check if all values were correctly stored
  */

  // writting 1, 2, 3, 4, ... 16, into every word in block
  for (int i = 0; i < 16; ++i) {
    write(i*4, (uint8_t *)(&i));
  }

  // force write back in block
  read(0x00004000, (uint8_t *)&value1);

  // read back all blocks
  for (int i = 0; i < 16; ++i) {
    read(i*4, (uint8_t *)(&value1));
    assert((int)value1 == i && "Unexpected number stored in memory");
  }

  long total_time = getTime() - start_time;

  long expected_time = DRAM_READ_TIME + 16*L1_WRITE_TIME +
                        DRAM_READ_TIME + DRAM_WRITE_TIME + L1_READ_TIME +
                        DRAM_READ_TIME + 16*L1_READ_TIME;

  assert(total_time == expected_time && "Expected 2 write misses");

  printf("Successful test!\n");

  return 0;
}
