#include "../../Cache.h"
#include "../L2Cache.h"
#include <assert.h>
#include <stdint.h>

int main() {
  uint32_t value1;

  resetTime();
  initCache();

  long start_time = getTime();

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
  

  assert(result == 1 && "Expected 0 in 0x4000");

  long total_time = getTime() - start_time;

  long expectd_time = 
                  DRAM_READ_TIME + L2_READ_TIME + L1_WRITE_TIME +
                  DRAM_READ_TIME + L2_READ_TIME + L2_WRITE_TIME + L1_READ_TIME +
                  DRAM_READ_TIME + L2_READ_TIME + L1_READ_TIME +
                  L2_READ_TIME;


  // will result in 1 miss and 3 hits 
//   assert(total_time == expected_time);
  printf("Taken: %lu\n", total_time); 
  printf("Expected: %lu\n", expectd_time); 
  printf("Successful test!\n");
}