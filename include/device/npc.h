#ifndef __NPC_H__
#define __NPC_H__

#include "verilated.h"
#include "VCPU.h"
#include "VCPU___024root.h"

#include "vga.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class NPC; }
QT_END_NAMESPACE

extern uint32_t inst_from_npc;

class NPC: public QThread
{
    Q_OBJECT
public:
    NPC( int argc, char **argv, QObject* parent = nullptr);
    ~NPC();
    VCPU* cpu;
    NPCVGA *npc_vga;
    
    bool stop_flag;             // 监视点触发，引起中断
    bool debug_flag;
    bool diff_result;
    void init_npc();
    void exec(uint32_t n);

public slots:

protected:
    void run() override;

private:
    int argc;
    char **argv;
    int quit_flag;
    char logbuf[128];
    uint32_t last_pc;
    uint32_t cur_inst;

    void single_cycle();
    void reset(int n);
    bool checkMemBreakpointTrigger();
    bool checkRegBreakpointTrigger();

signals:
    void sendNPCInfoToUI(unsigned int pc);
    void sendRegInfo(unsigned int *reg_val);
    void destroyWindow();
    void sendDiffError();
    void updateMemory(unsigned int addr, unsigned int * vmem);
};

#endif