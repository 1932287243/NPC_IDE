#ifndef __UTILS_H__
#define __UTILS_H__

#include <assert.h>

#define log_write(...) IFDEF(CONFIG_TARGET_NATIVE_ELF, \
  do { \
    extern FILE* log_fp; \
    extern long inst_s_log_fp;\
    extern bool update_inst_file;\
    extern bool start_record_inst;\
    extern bool update_mem_log;\
    extern bool log_enable(); \
    if (log_enable()) { \
      if(update_inst_file==true)\
        fseek(log_fp, inst_s_log_fp, SEEK_SET);\
      if(update_mem_log==true)\
        fseek(log_fp, inst_s_log_fp, SEEK_SET);\
      fprintf(log_fp, __VA_ARGS__); \
      fflush(log_fp); \
      if(start_record_inst==false||update_mem_log==true)\
        inst_s_log_fp=ftell(log_fp);\
    } \
  } while (0) \
)

#define _Log(...) \
  do { \
    printf(__VA_ARGS__); \
    log_write(__VA_ARGS__); \
  } while (0)

#define Assert(cond, format, ...) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, format "\n", ## __VA_ARGS__); \
      assert(cond); \
    } \
  } while (0)
  
#define Perror(cond, format, ...) \
  Assert(cond, format ": %s", ## __VA_ARGS__, strerror(errno))

#define panic(...) Assert(0, __VA_ARGS__)


#endif