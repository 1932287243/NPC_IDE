#ifndef __SDB_H__
#define __SDB_H__

#include "common.h"
#include "main.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  char expr[32];
  bool enb;
  struct watchpoint *next;
} WP;

extern WP *head;

uint32_t expr(char *e, bool *success);
WP* new_wp();
void free_wp(WP *wp);
bool free_wp_no(int w_no);
void watch_info();
void sdb_mainloop();
void exec(uint32_t n);
extern int skip_batch_flag;

#endif
