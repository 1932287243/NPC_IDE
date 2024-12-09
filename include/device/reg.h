#ifndef __REG_H__
#define __REG_H__

#include <iostream>
#include <cstring>

class RISCV32Register;

extern RISCV32Register *riscv32_reg;

class RISCV32Register
{
public:

    // 寄存器名称数组
    const char *gpr_name[32] = {
        "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
        "fp", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
        "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
        "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
    };
    
    uint32_t gpr[32] = {0};     // 通用寄存器数组，初始化为0
    uint32_t pc;                // 程序计数器
    uint32_t csr[9];            // 控制状态寄存器

    RISCV32Register();
    ~RISCV32Register();
    uint32_t getGPRValue(std::string regName);
    void setGPRValue(std::string regName, uint32_t value);
    void initReg(uint32_t value);
    void printReg();
    void modifyReg(uint32_t index, uint32_t value);
};

#endif