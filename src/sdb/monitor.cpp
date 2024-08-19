#include "monitor.h"

void init_sdb();

static char *log_file = NULL;
static char *elf_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
int image_size;

static void welcome() {
  printf("Welcome to \33[1;33mriscv32e\33[0m-NEMU!\n");
  printf("For help, type \"help\"\n");
}

static int parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    // {"batch"    , no_argument      , NULL, 'b'},
    {"log"      , required_argument, NULL, 'l'},
    {"diff"     , required_argument, NULL, 'd'},
    {"ftrace"   , required_argument, NULL, 'f'},
    {"help"     , no_argument      , NULL, 'h'},
    {0          , 0                , NULL,  0 },
  };
  int o;
  while ( (o = getopt_long(argc, argv, "-hl:d:f:", table, NULL)) != -1) {
    switch (o) {
    //   case 'b': sdb_set_batch_mode(); break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 'f': elf_file = optarg; break;
      case 1: img_file = optarg; return 0;
      default:
        printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
        // printf("\t-b,--batch              run with batch mode\n");
        printf("\t-l,--log=FILE           output log to FILE\n");
        printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
        printf("\t-f,--ftrace=FILE        output function trace log to FILE\n");
        printf("\n");
        exit(0);
    }
  }
  return 0;
}

FILE *log_fp = NULL;

void init_log(const char *log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  printf("Log is written to %s\n", log_file ? log_file : "stdout");
}

static long load_img() {
    if (img_file == NULL) {
    printf("No image is given. Use the default build-in image.\n");
    return 4096; // built-in image size
    }

    FILE *fp = fopen(img_file, "rb");
    if(fp == NULL)
      printf("Can not open '%s'\n", img_file);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    printf("The image is %s, size = %ld\n", img_file, size);
    image_size = size;
    fseek(fp, 0, SEEK_SET);
    assert(mem_addr);
    int ret = fread(mem_addr, size, 1, fp);
    assert(ret == 1);

    fclose(fp);
    return size;
}

void init_monitor(int argc, char *argv[]) {
    /* Perform some global initialization. */

    /* Parse arguments. */
    parse_args(argc, argv);

    /* Open the log file. */
    init_log(log_file);

    /* Open the elf file. */
    init_ftrace(elf_file);
    // printf("elf_file=%s\n", elf_file);
    // for(int i=0; i<func_sym_num; i++)
    // {
    //   // if((addr>=func_sym_tab[i].func_addr) && (addr < (func_sym_tab[i].func_addr+func_sym_tab[i].func_size)))
    //   //   return i;
    //   printf("%s\n", func_sym_tab[i].func_name);
    // }

    /* Initialize memory. */
    init_mem();

    // /* Load the image to memory. This will overwrite the built-in image. */
    long img_size = load_img();

    // /* Initialize differential testing. */
    init_difftest(diff_so_file, img_size, mem_addr);

    // /* Initialize the simple debugger. */
    init_sdb();

    // init_disasm("riscv31-pc-linux-gnu");

    /* Display welcome message. */
    welcome();
}

