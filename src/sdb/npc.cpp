#include "npc.h"

static int trap_flag = 0;
static int abort_flag = 0;
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

NPC::NPC(int argc, char **argv, QObject *parent)
    : QThread(parent)
    , argc(argc)
    , argv(argv)
{
    top = new VCPU;
    last_pc = 0x80000000;
    cpu.pc = last_pc;
     for (int i = 0; i < 32; ++i) {
        cpu.gpr[i] = static_cast<unsigned int>(top->rootp->CPU__DOT__reg_file__DOT__rf[i]);
    }
}

NPC::~NPC()
{
    // Final model cleanup
    top->final();
    // Destroy model
    free_mem();
    delete top;
}

void NPC::single_cycle() {
  top->clk = 0; top->eval();
  top->clk = 1; top->eval();
}

void NPC::reset(int n) {
  top->rst = 1;
  while (n -- > 0) single_cycle();
  top->rst = 0;
}

void NPC::exec(uint32_t n)
{
  if(trap_flag == 0)
  {
    for(int i=0; i<n; i++)
    {
      top->inst = ((uint32_t *)mem_addr)[(top->pc - 0x80000000)/4]; 
      single_cycle();
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
      printf("%s\n",logbuf);
      emit sendNPCInfoToUI(last_pc, inst_from_npc);

      if(checkMemBreakpointTrigger())
        return;
      
      if(checkRegBreakpointTrigger())
        return;

      if(trap_flag||abort_flag)
      {
          if(trap_flag == 1)
            printf("\033[1;32mHIT GOOD TRAP\033[0m\n");
          else if(abort_flag == 1)
            printf("\033[1;31mHIT BAD TRAP\033[0m\n");
          break;
      }
      
      cpu.pc = top->pc;
      for (int i = 0; i < 32; ++i) {
        cpu.gpr[i] = static_cast<unsigned int>(top->rootp->CPU__DOT__reg_file__DOT__rf[i]);
      }
      diff_result = difftest_step(top->pc);
      if(diff_result==false)
      {
        printf("diff error\n");
        emit sendDiffError();
        return;
      }
      last_pc = top->pc;
    }
  }
  else
  {
    printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
  }
}

void NPC::init_npc()
{
    init_monitor(argc, argv);
    init_disasm("riscv32-pc-linux-gnu");
    // printf("0x%08x\n", ((uint32_t *)mem_addr)[1]);
    reset(5);
}

void NPC::run() 
{
  sdb_mainloop();
  emit destroyWindow();
}

 //判断是否触发内存的断点
bool NPC::checkMemBreakpointTrigger()
{
  for(int i = 0; i < w->mSplitterLayout->mem_breakpoint.size(); ++i)
  {
    QPair<int, int> pair = w->mSplitterLayout->mem_breakpoint.at(i);
    if(last_pc == (0x80000000 + pair.first*4))
    {
      last_pc = top->pc;
      return true;
    }
  }
  return false;
}

//判断是否触发寄存器的断点
bool NPC::checkRegBreakpointTrigger()
{
  for(int i = 0; i < w->mSplitterLayout->reg_breakpoint.size(); ++i)
  {
    QPair<int, int> pair = w->mSplitterLayout->reg_breakpoint.at(i);
    QTableWidgetItem *item_reg = w->mSplitterLayout->regTableWidget->item(pair.first, pair.second+1);
    if (item_reg) 
    {
      QString text = item_reg->text();  
      bool ok;
      uint32_t value = text.toUInt(&ok, 16); 
      if(value != static_cast<unsigned int>(top->rootp->CPU__DOT__reg_file__DOT__rf[pair.first*8+pair.second/2]))
      {
        last_pc = top->pc;
        return true;
      }
    }
  }
  return false;
}