#include "L2Cache.h"

#define INT_MAX 2147483647; // we want tidy algorithms

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
  uint32_t index, tag, mem_addr;
  uint8_t tmp_block[BLOCK_SIZE];

  /* init cache */
  if (l2_cache.init == 0) {
    for (int i = 0; i < L2_SIZE / (BLOCK_SIZE * L2_ASSOCIATIVTY); ++i) {
        for (int j = 0; j < L2_ASSOCIATIVTY ; ++j)
            l2_cache.lines[i][j].valid = 0;
    }
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
    index:  512 cache blocks => 2**9 / 2 sets = 2**8 => 8 index bits
    tag:    32 - 8 - 6 = 18 remaining bits
  */
  tag    = address >> 14;
  index  = (address >> 6) & 0xff; // 0xff = 0b1111 1111
  // offset = address & 0x3f;       // 0x3f = 0b0011 1111

  // find the line within the set and also the oldest line within the set
  CacheLine *line     = NULL;
  CacheLine *lru_line = NULL;
  int min_lru_time    = INT_MAX;
  int set_first_free = -1; // line for this block 
  int lru_index      = 0;
  int line_set_pos   = -1;

  for (int j = 0; j < L2_ASSOCIATIVTY; ++j) {
    CacheLine *current_line = &l2_cache.lines[index][j];

    // free position for block (if needed)
    if (!current_line->valid) {
        set_first_free = j;
    } else if (current_line->tag == tag) { // if valid and same tag, cache hit
        line = current_line;
        line_set_pos = j;
        break;
    } else if (current_line->time < min_lru_time) {
        lru_line = &l2_cache.lines[index][j];
        min_lru_time = lru_line->time;
        lru_index = j;
    }
  }

  /**
    Access Cache 
  */
  // if block not present, cache miss 
  if (!line) {
    // address of the desired block in memory
    mem_addr = (tag << 14) + (index << 6);
    accessDRAM(mem_addr, tmp_block, MODE_READ); // get new block from DRAM

    // if there is no free line, replace lru block
    if (set_first_free == -1) {
      // save dirty block
      if (lru_line->dirty) {
        mem_addr = (lru_line->tag << 14) + (index << 6);
        accessDRAM(mem_addr, &L2Blocks[(index + lru_index) * BLOCK_SIZE], MODE_WRITE);
      }
      set_first_free = lru_index;
    }

    // cache the new block 
    memcpy(&L2Blocks[(index + set_first_free) * BLOCK_SIZE], tmp_block, BLOCK_SIZE);
    line_set_pos = set_first_free;
    line = &l2_cache.lines[index][line_set_pos];
    line->valid = 1;
    line->tag = tag;
    line->dirty = 0;
  }

  if (mode == MODE_READ) {
    memcpy(data, &L2Blocks[(index + line_set_pos) * BLOCK_SIZE], BLOCK_SIZE);
    time += L2_READ_TIME;
  } else if (mode == MODE_WRITE) { // write data from cache line
    memcpy(&L2Blocks[(index + line_set_pos) * BLOCK_SIZE], data, BLOCK_SIZE);
    line->dirty = 1;
    time += L2_WRITE_TIME;
  }
  line->time = time;
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

    //   CacheLine l2_new_line = {
    //     .dirty = line->dirty,
    //     .tag   = mem_addr >> 15,
    //     .valid = 1,
    //   };

    //   int l2_index = (mem_addr >> 6) & 0x1ff;
    //   l2_cache.lines[l2_index] = l2_new_line;

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