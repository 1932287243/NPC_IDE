#include <readline/readline.h>
#include <readline/history.h>
#include <sys/select.h>
#include <unistd.h>

#include "sdb/sdb.h"
#include "vmem.h"
#include "reg.h"
#include "main.h"

SDB::SDB():cmd_table{
        {"help", "Display information about all supported commands"},
        {"c", "Continue the execution of the program"},
        {"q", "Exit NEMU"},
        {"si", "Step into N instructions"},
        {"info", "Display info about registers or watchpoints"},
        {"x", "Inspect memory at address"},
        {"p", "Evaluate an expression"},
        {"w", "Set a watchpoint"},
        {"d", "Delete a watchpoint"}
      }
{
    expr = new EXPR;
    line_read = nullptr;
    watchpoint = new WatchPointManager;
    stop_flag = false;
}

SDB::~SDB()
{
    delete expr;
    delete watchpoint;
}

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* SDB::rl_gets() {
    if (line_read) {
        free(line_read);
        line_read = nullptr;
    }
    line_read = readline("(npc-sdb)");
    if (line_read && *line_read) add_history(line_read);
    return line_read;
}

int SDB::cmd_c(char *args) {
    mNPC->exec(-1);
    return 0;
}

int SDB::cmd_si(char *args) {
    /* extract the first argument */
    char *arg = strtok(args, " ");
    int step_num;
    if (arg == NULL) 
    {
        step_num = 1;
        printf("USAGE: si N\n");
    }
    else
    {
        step_num = atoi(args);
        mNPC->exec(step_num);
    }
    return 0;
}

int SDB::cmd_info(char *args) {
    /* extract the first argument */
    char *arg = strtok(args, " ");
    if (arg == NULL) 
    {
        printf("info SUBCMD\n");
        return 0;
    }
    else if(*arg == 'r')
    {
        extern VCPU* top;
        for(int i=0; i<4; i++)
        {
            for(int j=0; j<8; j++)
            {
                printf("%s=0x%08x\t", riscv32_reg->gpr_name[i], riscv32_reg->gpr[8*i+j]);
            }
            printf("\n");
        }
    }
    else if(*arg == 'w')
    {
        watchpoint->printWatchPoints();
    }
    return 0;
}

int SDB::cmd_x(char *args) {
    uint32_t pmem_val;
    uint32_t base_addr;
    uint32_t pmem_addr;
    bool success;
    char *arg = args ? strtok(args, " "): nullptr;
    char* expr_ptr = arg ? strtok(nullptr, " ") : nullptr;

    if(arg==nullptr || expr_ptr==nullptr)
    {
        printf("Usage:x N EXPR\n");
        return 0;
    }
    else
    {
        int num = atoi(arg);
        base_addr = expr->evaluate(expr_ptr, success);
        if(base_addr < 0x80000000)
        {
            std::cout << "physical memory area [ 0x80000000 ,  0x87ffffff ] \n";
            return 0;
        }
        uint32_t i;
        uint32_t j;
        for(i=0; i<num; i++)
        {    
            printf("0x%08x:\t", base_addr);
            pmem_addr = base_addr;
            base_addr += 4;
            for(j=0; j<4; j++)
            {
                pmem_val = vmem->mem_addr[pmem_addr-0x80000000+j];
                printf("%02x\t", pmem_val);
            }
            printf("\n");
        }
    }
    return 0;
}

int SDB::cmd_p(char *args) {
    uint32_t val;
    if(args == NULL)
    {
        printf("Usage:p EXPR\n");
    }
    else
    {
        bool success;
        val = expr->evaluate(args, success);
        printf("\033[41;32mval=0x%08x\033[0m\n", val);
    }
    return 0;
}

int SDB::cmd_w(char *args) {
    if(args == NULL)
    {
        printf("Usage:w EXPR\n");
    }
    else
    {
        WatchPoint *wp;
        wp = watchpoint->newWatchPoint();
        wp->expr = args;
        std::cout << "\033[41mID:" << wp->NO << "\tEXPR:" << args << "\033[0m\n";
    }
    return 0;
}

int SDB::cmd_d(char *args) {
    if(args == nullptr)
    {
        printf("Usage:d N\n");
    }
    else
    {
        char *arg = strtok(args, " ");
        int w_num = atoi(arg);
        watchpoint->freeWatchPointByNo(w_num);
    }
    return 0;
}


void SDB::cmd_q(char *args) {
    return;
}

int SDB::cmd_help(char *args) {
    if (!args) {
        for (const auto& cmd : cmd_table) {
            std::cout << cmd.first << " - " << cmd.second << "\n";
        }
    } else {
        for (const auto& cmd : cmd_table) {
            if (cmd.first == args) {
                std::cout << cmd.first << " - " << cmd.second << "\n";
                return 0;
            }
        }
        std::cout << "Unknown command: " << args << "\n";
    }
    return 0;
}

int SDB::sdb_mainloop() {
    if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH)
    {
        fd_set readfds;
        struct timeval timeout;
        char* input;
        bool show_start = true;

        // 设置超时时间
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000;  // 500毫秒

        // 清空文件描述符集合，并设置stdin（标准输入）为监听目标
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        while (true) {
            // 使用 select 来检测标准输入是否有数据
            int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

            if (ret == -1) {
                perror("select failed");
                break;
            } 
            else if (ret > 0) {
               
                std::string line;

                char* line_read = readline("");  

                if (!line_read) break;
                char* cmd = strtok(line_read, " ");
                char* args = cmd ? strtok(nullptr, "") : nullptr;
                if (cmd){
                    if(executeCommand(cmd, args)==-1)
                    {
                        return -1;
                    }
                }
                show_start = true;
            } 
            else {
                if(stop_flag)
                {
                    std::cout << "No input available, doing other work..." << std::endl;
                    break;
                } 
                if(show_start)
                {
                    std::cout << "(npc-sdb) "<< std::flush;
                    show_start = false;
                }
            }

            // 每次循环都需要重新设置文件描述符集合
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
        }
    }

    if(mode_sel == DISPLAY_SDB_ONLY)
    {
        while (true) {
            char* line = rl_gets();
            if (!line) break;
            char* cmd = strtok(line, " ");
            char* args = cmd ? strtok(nullptr, "") : nullptr;
            if (cmd){
                if(executeCommand(cmd, args)==-1)
                {
                    return -1;
                }
            }        
        }
    }
   return 0;
}

int SDB::executeCommand(const std::string& cmd, char* args) {
    if (cmd == "help") {
        cmd_help(args);
    } else if (cmd == "c") {
        cmd_c(args);
    } else if (cmd == "q") {
        cmd_q(args);
        return -1;
    } else if (cmd == "si"){
        cmd_si(args);
    } else if (cmd == "info"){
        cmd_info(args);
    } else if (cmd == "x"){
        cmd_x(args);
    } else if (cmd == "p"){
        cmd_p(args);
    } else if (cmd == "w"){
        cmd_w(args);
    } else if (cmd == "d"){
        cmd_d(args);
    }else {
        std::cout << "Unknown command: " << cmd << "\n";
    }
    return 0;
}

void SDB::init_sdb() {
    expr->initExpr();
    watchpoint->initPool();
}
