#include "difftest.h"

riscv32_CPU_state cpu;

void (*ref_difftest_memcpy)(uint32_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;

void init_difftest(char *ref_so_file, long img_size, uint8_t *mem_addr) {
  assert(ref_so_file != NULL);

  void *handle;
  handle = dlopen(ref_so_file, RTLD_LAZY);
  assert(handle);

  ref_difftest_memcpy = (void (*)(uint32_t, void*, size_t, bool))dlsym(handle, "difftest_memcpy");
  assert(ref_difftest_memcpy);

  ref_difftest_regcpy = (void (*)(void*, bool))dlsym(handle, "difftest_regcpy");
  assert(ref_difftest_regcpy);

  ref_difftest_exec = (void (*)(uint64_t))dlsym(handle, "difftest_exec");
  assert(ref_difftest_exec);

  ref_difftest_raise_intr = (void (*)(uint64_t))dlsym(handle, "difftest_raise_intr");
  assert(ref_difftest_raise_intr);

  void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
  assert(ref_difftest_init);

//   Log("", ANSI_FMT("ON", ANSI_FG_GREEN));
  printf("Differential testing: \033[1,31mON\033[0m\n");
  printf("The result of every instruction will be compared with %s. "
      "This will help you a lot for debugging, but also significantly reduce the performance. "
      "If it is not necessary, you can turn it off in menuconfig.\n", ref_so_file);

  ref_difftest_init(0);
  ref_difftest_memcpy(0x80000000, mem_addr, img_size, true);
  ref_difftest_regcpy(&cpu, true);
}

bool isa_difftest_checkregs(riscv32_CPU_state *ref_r, uint32_t pc) {
    if(pc != ref_r->pc)
        return false;
    for(int i = 0; i<32; i++)
    {
        if(ref_r->gpr[i] != cpu.gpr[i])
        return false;
    }
    return true;
}

bool difftest_step(uint32_t pc) {
    riscv32_CPU_state ref_r;
    ref_difftest_exec(1);
    ref_difftest_regcpy(&ref_r, false);
    if(isa_difftest_checkregs(&ref_r, pc))
        return true;
    else
        return false; 
}

