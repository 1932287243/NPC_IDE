#include "vmem.h"

static uint8_t *pmem = NULL;
uint8_t *mem_addr = NULL;

void init_mem() {
    pmem = (uint8_t *)malloc(CONFIG_MSIZE);
    assert(pmem);
    memset(pmem, 0, CONFIG_MSIZE);
    mem_addr =pmem;
    printf("physical memory area [ 0x%08x ,  0x%08x ]\n", PMEM_LEFT, PMEM_RIGHT);
}

void free_mem()
{
    free(pmem);
}
