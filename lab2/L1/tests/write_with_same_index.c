#include "../../Cache.h"
#include "../L1Cache.h"
#include <assert.h>

int main() {
  uint32_t value1;
  resetTime();
  initCache();

  long start_time = getTime();
  /**
  Use two memory addresses that will conflict in the same cache line, then
  read from the address that was replaced by the second
  */
  value1 = 1;
  write(0, (uint8_t *)(&value1));


  value1 = 2;
  write(0x00004000, (uint8_t *)(&value1));

  read(0, (uint8_t *)(&value1));

  /**
  Should result in:
  1 DRAM Read - compulsory miss, fetch block to write first value
  1 Cache Write - writting value in block1 into cache
  2 DRAM Accesses - write dirty block, fetch new one with same index
  1 Cache Write - writting value ito new block into cache
  1 DRAM Access - fetch block that was replaced
  1 Cache Read - read value from recently fetched block
  */ 
  long total_time = getTime() - start_time;
  long expected_time = DRAM_READ_TIME + L1_WRITE_TIME +
                        DRAM_READ_TIME + DRAM_WRITE_TIME + L1_WRITE_TIME +
                        DRAM_READ_TIME + DRAM_WRITE_TIME + L1_READ_TIME;

  assert(total_time == expected_time && 
                          "4DRAM acesses, 2 Cache Writes and 1 Cache Read");
  assert(value1 == 1 && "Should fetch first value written");

  printf("Successful test!\n");

  return 0;
}
