#ifndef __DISASM_H__
#define __DISASM_H__

#include "common.h"
extern "C" {
  extern void init_disasm(const char *triple);
  extern void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
}
#endif
