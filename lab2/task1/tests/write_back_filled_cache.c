#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;
  // uint32_t value2;

  resetTime();
  initCache();

  long start_time = getTime();

  /** Fill all cache entries */
  for (int i = 0; i < 256; i++) {
    write(64*i, (uint8_t*)(&i));
  }

  /** Replace all blocks */
  for (int i = 0; i < 256; i++) {
    read(256*64 + 64*i, (uint8_t*)(&value1));
  }

  long total_time = getTime() - start_time;
  long expected_time = 256*DRAM_READ_TIME + 256*L1_READ_TIME +
                        256*DRAM_READ_TIME + 256*DRAM_WRITE_TIME + 256*L1_WRITE_TIME;

  assert(total_time == expected_time && "Should write to all blocks in cache and then write them back to memory");
  printf("Successful test!\n");

  return 0;
}
