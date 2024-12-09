#include "main.h"
#include "timer.h"
#include "reg.h"
#include "vmem.h"
#include "disasm.h"

static int trap_flag = 0;
static int abort_flag = 0;
static int inst_skip_flag = 0;

uint32_t inst_from_npc;

extern "C" void trap( int flag)
{
    trap_flag = flag;
}

extern "C" void npc_state ( int flag, unsigned int inst_to_sdb)
{
    abort_flag = flag;
    inst_from_npc = inst_to_sdb;
}


extern "C" int pmem_read(int raddr) {
    // 总是读取地址为`raddr & ~0x3u`的4字节返回

    // printf("raddr = %d\n", raddr);
    if(raddr == 0xa0000048)  // 获取当前时间
    {
        inst_skip_flag = 1;  // 差分测试跳过
        return (uint32_t)(NPCTimer::now_time);
    }
    if(raddr == 0xa000004c)
    {
        inst_skip_flag = 1;  // 差分测试跳过
        NPCTimer::now_time = NPCTimer::get_time();
        return (uint32_t)((NPCTimer::now_time) >> 32);
    }

    if(raddr == 0xa0000100)
    {
        return (uint32_t)((NPCVGA::screen_width << 16) | NPCVGA::screen_height);  // 获取宽高
    }

    if(raddr<0x80000000 || raddr > 0x88000000)   // 在一个时钟周期内多次访问内存
    {
        // 触发异常：未完成
        // printf("Address out of bounds:addr=0x%08x\n", raddr);
        return 0;
    }
    // printf("addr=0x%08x\tval=0x%08x\n", raddr, ((uint32_t *)vmem->mem_addr)[(raddr&(~0x3u) - 0x80000000u)/4]);
    return ((uint32_t *)vmem->mem_addr)[(raddr&(~0x3u) - 0x80000000u)/4];
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  // 总是往地址为`waddr & ~0x3u`的4字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变

    if(waddr == 0xa00003f8)
    {
        inst_skip_flag = 1;  // 差分测试跳过
        putchar(wdata);
        fflush(stdout);   //强制刷新缓冲区
        return;
    }

    if(waddr == 0xa0000104)  // 控制同步信号
    {
        NPCVGA::sync_signal = wdata;
        return;
    }

    if((waddr >= 0xa1000000) && (waddr <= (0xa1000000+NPCVGA::screen_width*NPCVGA::screen_height*4)))
    {
        memcpy((NPCVGA::vmem+(waddr-0xa1000000)/4), &wdata, 4);
        return;
    }

    if(waddr<0x80000000 || waddr > 0x88000000)   // 在一个时钟周期内多次访问内存
    {
        // 触发异常：未完成
        printf("Address out of bounds:addr=0x%08x\n", waddr);
        return;
    }

    switch (wmask){
        case 0b00001111:
            memcpy(vmem->mem_addr+(waddr&(~0x3u)-0x80000000), &wdata, 4);
            break;
        case 0b00001100:
            // printf("sh1100:\taddr = 0x%08x\t val = 0x%08x\n", waddr, wdata);
            // printf("old = 0x%08x\n", ((uint32_t *)mem_addr)[(waddr&(~0x3u)-0x80000000)/4]);
            memcpy(vmem->mem_addr+(waddr&(~0x3u)-0x80000000)+2, &wdata, 2);
            // printf("new = 0x%08x\n", ((uint32_t *)mem_addr)[(waddr&(~0x3u)-0x80000000)/4]);
            break;
        case 0b00000011:
            memcpy(vmem->mem_addr+(waddr&(~0x3u)-0x80000000), &wdata, 2);
            break;
        case 0b00000001:
            memcpy(vmem->mem_addr+(waddr-0x80000000), &wdata, 1);
            break;
        case 0b00000010:
            memcpy(vmem->mem_addr+(waddr-0x80000000), &wdata, 1);
            break;
        case 0b00000100:
            memcpy(vmem->mem_addr+(waddr-0x80000000), &wdata, 1);
            break;
        case 0b00001000:
            memcpy(vmem->mem_addr+(waddr-0x80000000), &wdata, 1);
            break;
        default:
            break;
    }
}

NPC::NPC(int argc, char **argv, QObject *parent)
    : QThread(parent)
    , argc(argc)
    , argv(argv)
{
    cpu = new VCPU;
    npc_vga = new NPCVGA(400, 300);   //创建显示
    npc_vga->start();                 //启动显示线程

    last_pc = 0x80000000;
    riscv32_reg->pc = last_pc;
    stop_flag = false;
    debug_flag = false;
    for (int i = 0; i < 32; ++i) {
        riscv32_reg->gpr[i] = static_cast<unsigned int>(cpu->rootp->CPU__DOT__reg_file__DOT__rf[i]);
    }

    for (int i = 0; i < 9; ++i) {
        riscv32_reg->csr[i] = static_cast<unsigned int>(cpu->rootp->CPU__DOT__csr_file__DOT__csrf[i]);
    }
}

NPC::~NPC(){
    // Final model cleanup
    cpu->final();
    npc_vga->stop();  // 请求线程退出
    npc_vga->wait();  // 等待线程退出
    delete npc_vga;
    delete cpu;
}

void NPC::single_cycle() {
    cpu->clk = 0; cpu->eval();
    cpu->clk = 1; cpu->eval();
}

void NPC::reset(int n) {
    cpu->rst = 1;
    while (n -- > 0) single_cycle();
    cpu->rst = 0;
}

void NPC::exec(uint32_t n){
    bool success;
    if(trap_flag == 0){
        for(int i=0; i<n; i++){

            cpu->inst = ((uint32_t *)vmem->mem_addr)[(cpu->pc - 0x80000000)/4]; 
            single_cycle();
            if(debug_flag)
            {
                // 判断是否是批处理模式
                if(mode_sel != BATCH_MODE)
                {
                    char *p = logbuf;
                    p += snprintf(p, sizeof(logbuf), "0x%08x:", last_pc);
                    int j;
                    uint8_t *inst = (uint8_t *)&inst_from_npc;
                    for (j = 0; j < 4; j++) {
                        p += snprintf(p, 4, " %02x", inst[j]);
                    }
                    memset(p, ' ', 1);
                    p += 1;
                    disassemble(p, logbuf + sizeof(logbuf) - p, last_pc, (uint8_t *)&inst_from_npc, 4);
                    // printf("%s\n",logbuf);
                    std::cout << logbuf << std::endl;

                    if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH )
                        emit sendNPCInfoToUI(last_pc);

                    // copy cpu state
                    riscv32_reg->pc = cpu->pc;
                    for (int i = 0; i < 32; ++i) {
                        riscv32_reg->gpr[i] = static_cast<unsigned int>(cpu->rootp->CPU__DOT__reg_file__DOT__rf[i]);
                    }
                    for (int i = 0; i < 9; ++i) {
                        riscv32_reg->csr[i] = static_cast<unsigned int>(cpu->rootp->CPU__DOT__csr_file__DOT__csrf[i]);
                        // printf("0x%08x\t", cpu.csr[i]);
                    }

                    if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH )
                        emit sendRegInfo( riscv32_reg->gpr);

                    // difftest
                    diff_result = monitor->difftest->step(cpu->pc, inst_skip_flag);
                    if(diff_result==false)
                    {
                        printf("diff error\n");
                        // emit sendDiffError();
                        return;
                    }
                }

                // inst_skip_flag = 0;
                if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH)
                {
                    // last_pc = cpu->pc;
                    if(checkMemBreakpointTrigger())
                        return;
                    if(checkRegBreakpointTrigger())
                        return;
                }

                last_pc = cpu->pc;

                if(mode_sel != BATCH_MODE)
                {
                    // 判断是否触发监视点
                    WatchPoint* temp = monitor->sdb->watchpoint->head;
                    while (temp != nullptr) 
                    {
                        if(monitor->sdb->expr->evaluate(temp->expr, success))
                        {
                            std::cout << "\033[;41mMonitoring point trigger !!!\033[0m\n";
                            std::cout << "\t\033[;32mNO:" << temp->NO << "\tEXPR:" << temp->expr << "\033[0m\n";
                            return;
                        }
                        temp = temp->next;
                    }
                }
            }
            if(trap_flag||abort_flag){
                if(trap_flag == 1)
                    printf("\033[1;32mHIT GOOD TRAP\033[0m\n");
                else if(abort_flag == 1)
                    printf("\033[1;31mHIT BAD TRAP\033[0m\n");
                break;
            }
        }
    }
    else{
        printf("Program execution has ended. To restart the program, exit NPC and run again.\n");
    }
}

void NPC::init_npc()
{
    init_disasm("riscv32-pc-linux-gnu");
    reset(5);
}

void NPC::run() 
{
    exec(-1);
    // if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH || mode_sel == BATCH_MODE)
    //     emit destroyWindow();
}

 //判断是否触发内存的断点
bool NPC::checkMemBreakpointTrigger()
{
    // std::cout << ui->mSplitterLayout->mem_breakpoint.size() << std::endl;
    for(int i = 0; i < ui->mSplitterLayout->instMemWidget->mem_breakpoint.size(); ++i){
        QPair<int, int> pair = ui->mSplitterLayout->instMemWidget->mem_breakpoint.at(i);
        if(last_pc == (0x80000000 + pair.first*4)){
            last_pc = cpu->pc;
            emit sendDiffError();
            return true;
        }
    }
    return false;
}

//判断是否触发寄存器的断点
bool NPC::checkRegBreakpointTrigger()
{
    for(int i = 0; i < ui->mSplitterLayout->gprWidget->gpr_breakpoint.size(); ++i){
        QPair<int, int> pair = ui->mSplitterLayout->gprWidget->gpr_breakpoint.at(i);
        QTableWidgetItem *item_reg = ui->mSplitterLayout->gprWidget->gprTableWidget->item(pair.first, pair.second+1);
        if (item_reg) {
            QString text = item_reg->text();  
            bool ok;
            uint32_t value = text.toUInt(&ok, 16); 
            if(value != static_cast<unsigned int>(cpu->rootp->CPU__DOT__reg_file__DOT__rf[pair.first*8+pair.second/2])){
                last_pc = cpu->pc;
                return true;
            }
        }
    }
  return false;
}