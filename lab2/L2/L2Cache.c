#include "L2Cache.h"

uint8_t L1Blocks[L1_SIZE];
uint8_t L2Blocks[L2_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
L1Cache l1_cache;
L2Cache l2_cache;

void initCache() { l1_cache.init = 0; l2_cache.init = 0; }

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }
uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

/************************L2 cache **************************/
void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, tag, offset, mem_addr;
  uint8_t tmp_block[BLOCK_SIZE];

  /* init cache */
  if (l2_cache.init == 0) {
    for (int i = 0; i < L2_SIZE / BLOCK_SIZE; ++i)
      l2_cache.lines[i].valid = 0;
    l2_cache.init = 1;
  }

  /**
  address: [ tag 31-14 ; index 13-6; offset 5-0]
    tag:    18-bits
    index:  8-bits
    offset: 6-bits
  
  exp:
    offset: 16 words per block, words of size 4B => 16*4B per block => 2**6 B => 
      => 6 offset bits
    index:  512 cache blocks => 2**9 blocks => 9 index bits (direct mapped cache)
    tag:    32 - 9 - 6 = 19 remaining bits
  */
  tag    = address >> 15;
  index  = (address >> 6) & 0x1ff; // 0xff = 0b1111 1111
  offset = address & 0x3f;       // 0x3f = 0b0011 1111

  // current line being accessed
  CacheLine *line = &l2_cache.lines[index];

  /**
    Access Cache 
  */
  // if block not present, cache miss 
  if (!line->valid || line->tag != tag) {
    // address of the desired block in memory
    mem_addr = tag << 15;
    accessDRAM(mem_addr, tmp_block, MODE_READ); // get new block from DRAM

    // if line has dirty block
    if ((line->valid) && (line->dirty)) {
      // memory address of the dirty block in cache
      mem_addr = line->tag << 15;
      // write dirty block into memory
      accessDRAM(mem_addr, &L1Blocks[index], MODE_WRITE);
    }

    // cache the new block 
    memcpy(&L2Blocks[index * BLOCK_SIZE], tmp_block, BLOCK_SIZE);
    line->valid = 1;
    line->tag = tag;
    line->dirty = 0;
  }

  if (mode == MODE_READ) {
    memcpy(data, &L2Blocks[index * BLOCK_SIZE], BLOCK_SIZE);
    time += L2_READ_TIME;
  } else if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&L2Blocks[index * BLOCK_SIZE], data, BLOCK_SIZE);
    line->dirty = 1;
    time += L2_WRITE_TIME;
  }
}

/*********************** L1 cache *************************/
void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, tag, offset, mem_addr;
  uint8_t tmp_block[BLOCK_SIZE];

  /* init cache */
  if (l1_cache.init == 0) {
    for (int i = 0; i < L1_SIZE / BLOCK_SIZE; ++i)
      l1_cache.lines[i].valid = 0;
    l1_cache.init = 1;
  }

  /**
  address: [ tag 31-14 ; index 13-6; offset 5-0]
    tag:    18-bits
    index:  8-bits
    offset: 6-bits
  
  exp:
    offset: 16 words per block, words of size 4B => 16*4B per block => 2**6 B => 
      => 6 offset bits
    index:  256 cache blocks => 2**8 blocks => 8 index bits (direct mapped cache)
    tag:    32 - 8 - 6 = 18 remaining bits
  */
  tag    = address >> 14;
  index  = (address >> 6) & 0xff; // 0xff = 0b1111 1111
  offset = address & 0x3f;       // 0x3f = 0b0011 1111

  // current line being accessed
  CacheLine *line = &l1_cache.lines[index];

  /**
    Access Cache 
  */
  // if block not present, cache miss 
  if (!line->valid || line->tag != tag) {
    accessL2(address, tmp_block, MODE_READ); // get new block from L2 

    // evict to l2
    if ((line->valid)) { 
      mem_addr = (line->tag << 14) + (index << 6);

      CacheLine l2_new_line = {
        .dirty = line->dirty,
        .tag   = mem_addr >> 15,
        .valid = 1,
      };

      int l2_index = (mem_addr >> 6) & 0x1ff;
      l2_cache.lines[l2_index] = l2_new_line;

      accessL2(mem_addr, &L1Blocks[index * BLOCK_SIZE], MODE_WRITE);
    }

    // cache the new block 
    memcpy(&L1Blocks[index * BLOCK_SIZE], tmp_block, BLOCK_SIZE);
    line->valid = 1;
    line->tag = tag;
    line->dirty = 0;
  }

  int word_offset = WORD_SIZE * (offset >> 2);
  if (mode == MODE_READ) {
    memcpy(data, &L1Blocks[index * BLOCK_SIZE + word_offset], WORD_SIZE);
    time += L1_READ_TIME;
  } else if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&L1Blocks[index * BLOCK_SIZE + word_offset], data, WORD_SIZE);
    line->dirty = 1;
    time += L1_WRITE_TIME;
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
