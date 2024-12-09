#ifndef __SDB_H__
#define __SDB_H__

#include <iostream>
#include <vector>
#include "expr.h"
#include "watchpoint.h"

class EXPR;
class WatchPointManager;

class SDB
{
public:
    SDB();
    ~SDB();
    void init_sdb();                // init sdb
    int sdb_mainloop();            // sdb cmd
    int skip_batch_flag;
    EXPR *expr;
    WatchPointManager *watchpoint;
    bool stop_flag;

private:
    int cmd_help(char *args);       // help cmd
    void cmd_q(char *args);          // quit cmd
    int cmd_d(char *args);          // delete monitor point
    int cmd_w(char *args);          // set monitor point
    int cmd_p(char *args);          // expression evaluation
    int cmd_x(char *args);          // Scan Memory
    int cmd_info(char *args);       // Print register status and monitoring point information
    int cmd_si(char *args);         // Single step execution
    int cmd_c(char *args);          // continue to operate
    int executeCommand(const std::string& cmd, char* args);
    char* rl_gets();
    char *line_read;

    std::vector<std::pair<std::string, std::string>> cmd_table;
    
};

#endif
