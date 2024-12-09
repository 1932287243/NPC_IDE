#include "reg.h"

RISCV32Register *riscv32_reg = new RISCV32Register;

RISCV32Register::RISCV32Register()
{

}

RISCV32Register::~RISCV32Register()
{
    std::cout << "delete reg\n";
}

// 根据寄存器名获取寄存器值
uint32_t RISCV32Register::getGPRValue(std::string regName) {
    if(regName == "pc")
        return pc;
        
    for (int i = 0; i < 32; ++i) {
        if (regName == gpr_name[i]) {
            return gpr[i];
        }
    }
    std::cerr << "Error: Register name not found!" << std::endl;
    return 0;
}

// 修改寄存器值
void RISCV32Register::setGPRValue(std::string regName, uint32_t value) {
    for (int i = 0; i < 32; ++i) {
        if (regName==gpr_name[i]) {
            gpr[i] = value;
            return;
        }
    }
    std::cerr << "Error: Register name not found!" << std::endl;
}

// 初始化所有寄存器
void RISCV32Register::initReg(uint32_t value) {
    for (int i = 0; i < 32; ++i) {
        gpr[i] = value;
    }
}

// 打印所有寄存器的值
void RISCV32Register::printReg() {
    for (int i = 0; i < 32; ++i) {
        std::cout << gpr_name[i] << ": " << gpr[i] << std::endl;
    }
}

// 修改指定位置的寄存器（按顺序修改）
void RISCV32Register::modifyReg(uint32_t index, uint32_t value) {
    if (index < 32) {
        gpr[index] = value;
    } else {
        std::cerr << "Error: Invalid register index!" << std::endl;
    }
}