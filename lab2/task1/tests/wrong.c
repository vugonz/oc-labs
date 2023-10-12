#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();
  /**
  Write 4 words into the same block, expecting 1 miss and 3 cache hits
  */
  value1 = 1;
  write(1024, (uint8_t *)(&value1));

}