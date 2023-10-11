#ifndef L2CACHE_H 
#define L2CACHE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../Cache.h"

void resetTime();

uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL1(uint32_t, uint8_t *, uint32_t);
void accessL2(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine {
  uint8_t valid;
  uint8_t dirty;
  uint32_t tag;
  int time;
} CacheLine;

typedef struct L1Cache {
  uint32_t init;
  /** 
    nr blocks == cache size / block size
    in this case, 256 blocks, see Cache.h
  */
  CacheLine lines[L1_SIZE / BLOCK_SIZE];
} L1Cache;


typedef struct L2Cache {
  uint32_t init;
  /** 
    nr blocks == cache size / block size
    in this case, 256 blocks, see Cache.h
  */
  CacheLine lines[L2_SIZE / (BLOCK_SIZE * L2_ASSOCIATIVTY)][L2_ASSOCIATIVTY];
} L2Cache;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
