#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();

  /** 
  This is our big stress test, touch every memory word!! :nerdemoji:

  We fill the cache with dirty blocks and then replace them all with more
  dirty blocks untill all memory is touched
  */

  // fill cache with fully written blocks and force write back on every new fill
  for (int x = 0; x < 4; ++x) {
    for (int i = 0; i < 256; ++i) {
      for (int j = 0; j < 16; ++j) {
        write(16384*x + 64*i + 4*j, (uint8_t *)(&j));
      }
    }
  }

  // last but not least, force write back on the last blocks loaded
  for (int i = 0; i < 256; ++i) {
    read(64*i, (uint8_t *)(&value1));
  }


  long total_time = getTime() - start_time;
  long expected_time = 
        256*DRAM_READ_TIME + 256*16*L1_WRITE_TIME + // first fill
      + 3*256*(DRAM_READ_TIME  + 16*L1_WRITE_TIME + DRAM_WRITE_TIME) + // subsequent fills and write backs
      + 256*DRAM_WRITE_TIME + 256*DRAM_READ_TIME + 256*L1_READ_TIME; // last write back

  assert(total_time == expected_time && "Expects a memory access for every block in cache, twice!");
  printf("Successful test!\n");

  return 0;
}
