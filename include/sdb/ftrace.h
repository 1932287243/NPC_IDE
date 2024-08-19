#ifndef __FTRACE_H__
#define __FTRACE_H__

#include "common.h"
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct
{
    char func_name[20];
    uint32_t func_addr;
    uint32_t func_size;
}Func_Sym;


extern Func_Sym func_sym_tab[40];
extern int func_sym_num;
extern FILE *elf_fp;

void init_ftrace(const char *elf_file);

#endif