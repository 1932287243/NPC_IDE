#ifndef FTRACE_H
#define FTRACE_H

#include <iostream>
#include <vector>
#include <elf.h>

class Ftrace {
public:
    struct FuncSym {
        char func_name[20];
        uint32_t func_addr;
        uint32_t func_size;
    };

    Ftrace() : elf_fp(nullptr), func_sym_num(0) {}

    ~Ftrace() {
        std::cout << "delete ftrace\n";
        if (elf_fp) {
            fclose(elf_fp);
        }
    }
    int func_sym_num;
    std::vector<FuncSym> func_sym_tab;
    std::string src;

    void initFtrace(const std::string &elf_file);
    int instTotal(const std::string &elf_file);
    void getSourceCode(const std::string &elf_file);

private:
    FILE *elf_fp;

    Elf32_Ehdr ehdr;
    Elf32_Shdr symtab_shdr;
    Elf32_Shdr strtab_shdr;

    void parseElfSymbols(const std::string &filename);
    void getElfHeader();
    void getElfSection();
    void getElfSymbols();
};

#endif // FTRACE_H