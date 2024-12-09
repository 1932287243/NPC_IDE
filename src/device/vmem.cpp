#include "vmem.h"
#include <cassert>
#include <cstring>

#define CONFIG_MBASE 0x80000000
#define CONFIG_MSIZE 0x8000000
#define PMEM_LEFT  ((uint32_t)CONFIG_MBASE)
#define PMEM_RIGHT ((uint32_t)CONFIG_MBASE + CONFIG_MSIZE - 1)

Vmem *vmem = new Vmem;

Vmem::Vmem()
{
}

Vmem::~Vmem()
{
    std::cout << "delete memory" << std::endl;
    free(mem_addr); 
}

void Vmem::init_mem() {
    mem_addr = (uint8_t *)malloc(CONFIG_MSIZE);
    assert(mem_addr);
    memset(mem_addr, 0, CONFIG_MSIZE);
    // mem_addr =pmem;
    printf("physical memory area [ 0x%08x ,  0x%08x ]\n", PMEM_LEFT, PMEM_RIGHT);
}

void Vmem::free_mem()
{
    free(mem_addr);
}
