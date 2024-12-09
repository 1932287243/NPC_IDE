#include "tools/difftest.h"
#include "reg.h"

DiffTest::DiffTest() 
{
    cpu.pc = riscv32_reg->pc;
    for (int i = 0; i < 32; i++) {
      cpu.gpr[i] = riscv32_reg->gpr[i];
    }
}

DiffTest::~DiffTest() 
{
    std::cout << "difftest" << std::endl;
    if (handle) {
        dlclose(handle);
    }
}

void DiffTest::initDifftest(const std::string& ref_so_file, size_t img_size, uint8_t* mem_addr) 
{
    this->img_size = img_size;
    this->mem_addr = mem_addr;
    assert(!ref_so_file.empty());

    handle = dlopen(ref_so_file.c_str(), RTLD_LAZY);
    assert(handle);

    ref_difftest_memcpy = (MemCpyFunc)dlsym(handle, "difftest_memcpy");
    ref_difftest_regcpy = (RegCpyFunc)dlsym(handle, "difftest_regcpy");
    ref_difftest_exec = (ExecFunc)dlsym(handle, "difftest_exec");
    ref_difftest_raise_intr = (RaiseIntrFunc)dlsym(handle, "difftest_raise_intr");
    auto ref_difftest_init = (InitFunc)dlsym(handle, "difftest_init");

    assert(ref_difftest_memcpy && ref_difftest_regcpy && ref_difftest_exec && ref_difftest_raise_intr && ref_difftest_init);

    std::cout << "Differential testing: \033[1;31mON\033[0m\n";
    std::cout << "The result of every instruction will be compared with " << ref_so_file
                << ". This will help you a lot for debugging but also significantly reduce the performance. "
                    "If it is not necessary, you can turn it off in menuconfig.\n";

    ref_difftest_init(0);
    ref_difftest_memcpy(0x80000000, mem_addr, img_size, true);
    ref_difftest_regcpy(&cpu, true);
}

bool DiffTest::checkRegisters(const RISCV32_CPUState& ref_r, uint32_t pc) const 
{
    if (pc != ref_r.pc) {
        std::cout << "pc=0x" << std::hex << pc << "\tref_pc=0x" << ref_r.pc << std::dec << "\n";
        return false;
    }
    
    for (int i = 0; i < 32; i++) {
        if (ref_r.gpr[i] != riscv32_reg->gpr[i]) {
            std::cout << "\033[1;31mreg" << i << ":dut=0x" << std::hex << riscv32_reg->gpr[i]
                        << "\tref=0x" << ref_r.gpr[i] << "\033[0m\n"
                        << std::dec;
            return false;
        }
    }

    return true;
}

bool DiffTest::step(uint32_t pc, int inst_skip_flag) 
{
    RISCV32_CPUState ref_r;

    if (inst_skip_flag) {
        // 差分测试跳过
        ref_difftest_memcpy(0x80000000, mem_addr, img_size, true);
        ref_difftest_regcpy(&cpu, true);
        return true;
    }

    ref_difftest_exec(1);
    ref_difftest_regcpy(&ref_r, false);

    return checkRegisters(ref_r, pc);
}


