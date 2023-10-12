#include "../../Cache.h"
#include "../L2Cache.h"
#include <assert.h>
#include <stdint.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  /**
  Test that writes back a L2 block
  */
  value1 = 1;
  write(0x4000, (uint8_t *)(&value1));

  // conflict in both l1 and l2
  read(0xc000, (uint8_t *)(&value1));

  read(0x8000, (uint8_t *)(&value1));

  uint8_t result = 0;
  read(0x4000, (uint8_t* )(&result));


  assert(result == 1);
  printf("Successful test");
}
