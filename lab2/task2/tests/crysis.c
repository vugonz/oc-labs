#include "../../Cache.h"
#include "../L2Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  /** 
  This is our big stress test, touch every memory word!! :nerdemoji:

  We fill the cache with dirty blocks and then replace them all with more
  dirty blocks untill all memory is touched
  */

  uint32_t value = 1;
  write(0, (uint8_t *)(&value));

  value1 = 1;
  write(800, (uint8_t *)(&value));

  value1 = 1;
  write(123, (uint8_t *)(&value));

  value1 = 1;
  write(4928, (uint8_t *)(&value));

  value1 = 1;
  write(12093, (uint8_t *)(&value));

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
  assert(value == 1);

  read(800, (uint8_t *)(&value));
  printf("Result: %d\n", value);
  assert(value == 1);

  read(123, (uint8_t *)(&value));
  printf("Result: %d\n", value);
  assert(value == 1);

  read(4928, (uint8_t *)(&value));
  printf("Result: %d\n", value);
  assert(value == 1);

  read(12093, (uint8_t *)(&value));
  printf("Result: %d\n", value);
  assert(value == 1);

  printf("Successful test!\n");

  return 0;
}
