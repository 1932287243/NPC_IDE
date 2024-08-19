#ifndef __VMEM_H__ 
#define __VMEM_H__

#include "common.h"

#define CONFIG_MBASE 0x80000000
#define CONFIG_MSIZE 0x8000000
#define PMEM_LEFT  ((uint32_t)CONFIG_MBASE)
#define PMEM_RIGHT ((uint32_t)CONFIG_MBASE + CONFIG_MSIZE - 1)

extern uint8_t *mem_addr;

void init_mem(); 
void free_mem();
#endif