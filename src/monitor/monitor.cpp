#include "main.h"
#include "vmem.h"
#include "reg.h"

Monitor::Monitor(std::string log_file, std::string elf_file, std::string diff_so_file, std::string img_file):
    log_file(log_file),
    elf_file(elf_file),
    diff_so_file(diff_so_file),
    img_file(img_file)
{
    difftest = new DiffTest;
    ftrace = new Ftrace;
    sdb = new SDB;
    stop_flag = false;
}

Monitor::~Monitor()
{
    delete ftrace;
    delete difftest;
    delete sdb;
}

void Monitor::welcome() const {
    std::printf("Welcome to \33[1;33mriscv32e\33[0m-NPC!\n");
    std::printf("For help, type \"help\"\n");
}

void Monitor::initLog() {
    log_fp = stdout;
    if (!log_file.empty()) {
        log_fp = std::fopen(log_file.c_str(), "w");
        assert(log_fp && "Cannot open log file");
    }
    std::printf("Log is written to %s\n", log_file.empty() ? "stdout" : log_file.c_str());
}

long Monitor::loadImage() {
    if (img_file.empty()) {
        std::printf("No image is given. Use the default built-in image.\n");
        return 4096;  // Default image size
    }

    FILE* fp = std::fopen(img_file.c_str(), "rb");
    if (!fp) {
        std::printf("Cannot open '%s'\n", img_file.c_str());
        return -1;
    }

    std::fseek(fp, 0, SEEK_END);
    long size = std::ftell(fp);
    std::printf("The image is %s, size = %ld\n", img_file.c_str(), size);
    image_size = size;
    std::fseek(fp, 0, SEEK_SET);

    assert(vmem->mem_addr);
    int ret = std::fread(vmem->mem_addr, size, 1, fp);
    assert(ret == 1);

    std::fclose(fp);
    return size;
}

void Monitor::init_monitor(int argc, char* argv[]) {
    // Parse arguments
    // parseArgs(argc, argv);

    // Open the log file
    initLog();

    // Initialize elf file and fetch instruction numbers
    ftrace->initFtrace(elf_file);
    inst_num = ftrace->instTotal(elf_file);
    ftrace->getSourceCode(elf_file);

    // std::cout << "inst_num=" << inst_num << std::endl;
    // for(int i=0; i<ftrace->func_sym_num; i++)
    // {
    //     std::cout << ftrace->func_sym_tab[i].func_name << std::endl;
    // }

    // Initialize memory
    vmem->init_mem();

    // Load image into memory
    loadImage();

    // Initialize differential testing
    difftest->initDifftest(diff_so_file.c_str(), image_size, vmem->mem_addr);

    // Initialize simple debugger
    sdb->init_sdb();

    // Display welcome message
    welcome();
}

void Monitor::stop()
{
    sdb->stop_flag = true;
}

void Monitor::run() 
{
   
    int ret = sdb->sdb_mainloop();
    if(ret == -1)
    {
        if(mode_sel == DISPLAY_SDB_ONLY)
        {
            stop_flag = true;
        }
    
        if(mode_sel == DISPLAY_IDE_ONLY || mode_sel == DISPLAY_BOTH || mode_sel == BATCH_MODE)
            emit destroyWindow();
    }
}


