#ifndef __MONITOR_H__
#define __MONITOR_H__
#include "utils.h"
#include "common.h"
#include "vmem.h"
#include "sdb.h"
#include "ftrace.h"
#include "difftest.h"
#include <getopt.h>

extern int image_size;
void init_monitor(int argc, char *argv[]);
// void init_log(const char *log_file);
// void init_ftrace(const char *elf_file) ;
// void init_difftest(char *ref_so_file, long img_size, int port);
// void init_disasm(const char *triple);

// void sdb_set_batch_mode();

#endif