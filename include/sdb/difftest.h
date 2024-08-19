#ifndef __DIFFTEST_H__
#define __DIFFTEST_H__

#include "common.h"
#include <dlfcn.h>

typedef struct {
  uint32_t gpr[32];
  uint32_t pc;
} riscv32_CPU_state;

extern riscv32_CPU_state cpu;

void init_difftest(char *ref_so_file, long img_size,  uint8_t *mem_addr);
bool difftest_step(uint32_t pc);

#endif // !__DIFFTEST_H__
