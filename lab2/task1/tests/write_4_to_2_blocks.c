#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;
  uint32_t value2;

  resetTime();
  initCache();

  value1 = 1;
  write(0, (uint8_t *)(&value1));

  value1 = 2;
  write(4, (uint8_t *)(&value1));

  value1 = 3;
  write(8, (uint8_t *)(&value1));

  value1 = 4;
  write(12, (uint8_t *)(&value1));

  value1 = 1;
  write(0 + BLOCK_SIZE, (uint8_t *)(&value1));

  value1 = 2;
  write(4 + BLOCK_SIZE, (uint8_t *)(&value1));

  value1 = 3;
  write(8 + BLOCK_SIZE, (uint8_t *)(&value1));

  value1 = 4;
  write(12 + BLOCK_SIZE, (uint8_t *)(&value1));

  read(0x8000, (uint8_t *)(&value1));
  read(0x8000 + BLOCK_SIZE, (uint8_t *)(&value1));

  read(4, (uint8_t *)(&value1));
  read(4 + BLOCK_SIZE, (uint8_t *)(&value2));

  assert(value1 == 2);
  assert(value2 == 2);

  printf("Successful test!\n");

  return 0;
}
