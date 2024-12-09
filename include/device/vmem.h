#ifndef __VMEM_H__ 
#define __VMEM_H__

#include <iostream>

class Vmem;

extern Vmem *vmem;

class Vmem
{
public:
    Vmem();
    ~Vmem();
    uint8_t *mem_addr;
    void init_mem(); 
    void free_mem();
};

#endif