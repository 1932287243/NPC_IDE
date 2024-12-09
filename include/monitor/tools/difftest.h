#ifndef DIFFTEST_H
#define DIFFTEST_H

#include <cassert>
#include <dlfcn.h>
#include <string>

#include <iostream>

struct RISCV32_CPUState {
    uint32_t gpr[32] = {0}; // 通用寄存器
    uint32_t pc = 0;        // 程序计数器
    uint32_t csr[9] = {0};  // 控制状态寄存器
};

class DiffTest {
private:
    void* handle = nullptr;

    // 函数指针
    using MemCpyFunc = void (*)(uint32_t, void*, size_t, bool);
    using RegCpyFunc = void (*)(void*, bool);
    using ExecFunc = void (*)(uint64_t);
    using RaiseIntrFunc = void (*)(uint64_t);
    using InitFunc = void (*)(int);

    MemCpyFunc ref_difftest_memcpy = nullptr;
    RegCpyFunc ref_difftest_regcpy = nullptr;
    ExecFunc ref_difftest_exec = nullptr;
    RaiseIntrFunc ref_difftest_raise_intr = nullptr;
    size_t img_size;
    uint8_t* mem_addr;

public:
    DiffTest() ;

    ~DiffTest();

    RISCV32_CPUState cpu;  // 当前 CPU 状态

    void initDifftest(const std::string& ref_so_file, size_t img_size, uint8_t* mem_addr);

    bool checkRegisters(const RISCV32_CPUState& ref_r, uint32_t pc) const;

    bool step(uint32_t pc, int inst_skip_flag);
};

#endif // DIFFTEST_H

