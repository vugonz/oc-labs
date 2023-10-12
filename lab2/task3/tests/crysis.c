#include "../../Cache.h"
#include "../L2Cache.h"
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

  uint32_t value = 1;
  write(0, (uint8_t *)(&value));

  value1 = 1;
  write(64, (uint8_t *)(&value));

  value1 = 1;
  write(128, (uint8_t *)(&value));

  value1 = 1;
  write(1024, (uint8_t *)(&value));

  // fill cache with fully written blocks and force write back on every new fill
  for (int x = 0; x < 4; ++x) {
    for (int i = 0; i < 256; ++i) {
      for (int j = 0; j < 16; ++j) {
        read(16384*x + 64*i + 4*j, (uint8_t *)(&value));
      }
    }
  }

 for (int i = 0; i < 256; ++i) {
    read(64*i, (uint8_t *)(&value1));
  }

  read(0, (uint8_t *)(&value));
  printf("Result: %d\n", value);

  read(64, (uint8_t *)(&value));
  printf("Result: %d\n", value);

  read(128, (uint8_t *)(&value));
  printf("Result: %d\n", value);

  read(1024, (uint8_t *)(&value));
  printf("Result: %d\n", value);

  // last but not least, force write back on the last blocks loaded
 


//   assert(total_time == expected_time && "Expects a memory access for every block in cache, twice!");
  printf("Successful test!\n");

  return 0;
}
