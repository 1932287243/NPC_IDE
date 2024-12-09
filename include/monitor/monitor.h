#ifndef MONITOR_H
#define MONITOR_H

#include <getopt.h>
#include <QThread>
#include <iostream>

#include "sdb/sdb.h"
#include "tools/ftrace.h"
#include "tools/difftest.h"

class SDB;
class Ftrace;
class DiffTest;

class Monitor:public QThread
{
    Q_OBJECT
public:
    Monitor(std::string log_file, std::string elf_file, std::string diff_so_file, std::string img_file);
    ~Monitor();
    DiffTest *difftest;
    Ftrace *ftrace;
    SDB *sdb;
    int image_size;
    int inst_num;
    bool stop_flag;
    void init_monitor(int argc, char* argv[]);
    void stop();


protected:
    void run() override;

private:
    // Data members
    int argc;
    char **argv;
    std::string log_file;
    std::string elf_file;
    std::string diff_so_file;
    std::string img_file;
    FILE* log_fp;

    // Member functions
    void welcome() const;

    void initLog();

    long loadImage();

    void initDifftest(long img_size) const;

signals:
    void destroyWindow();

};

#endif  // MONITOR_H
