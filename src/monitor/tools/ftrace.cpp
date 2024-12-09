#include "tools/ftrace.h"
#include <cstring>

#include <memory>
#include <array>

void Ftrace::initFtrace(const std::string &elf_file) 
{
    elf_fp = stdout;
    if (!elf_file.empty()) {
        elf_fp = fopen(elf_file.c_str(), "rb");
        if (elf_fp == nullptr) {
            std::cerr << "Cannot open '" << elf_file << "'" << std::endl;
            std::cout << "*****************" << std::endl;
        }
    }
    std::cout << "elf file:" << (elf_file.empty() ? "stdout" : elf_file) << std::endl;
    parseElfSymbols(elf_file);
}

int Ftrace::instTotal(const std::string &elf_file)
{
    char cmd[512];
    char buffer[128];
    int inst_num = 0;
    // 使用 sprintf 格式化命令
    sprintf(cmd,
            "riscv64-linux-gnu-objdump -d %s |"
            "sed -n -r '/^800[0-9a-f]{5}/p' |"
            "awk '{print $2}' |"
            "sed -n '/^[0-9a-f]/p' |"
            "wc -l",
            elf_file.c_str());

    FILE *fp = popen(cmd, "r");
    if (fp == nullptr) {
        std::cerr << "popen elf file error" << std::endl;
        return 0;
    }

    if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        inst_num = std::atoi(buffer);
        std::cout << "Total number of Instructions: " << inst_num << std::endl;
    }

    pclose(fp);
    return inst_num;
}

void Ftrace::getSourceCode(const std::string &elf_file)
{
    char cmd[512];
    // 创建一个管道，通过该管道读取外部命令的输出
    std::array<char, 128> buffer;
    // std::string result;
    // // 使用 sprintf 格式化命令
    sprintf(cmd,
            "riscv64-linux-gnu-objdump -d -S %s",
            elf_file.c_str());
    
    // 执行命令，并通过管道读取命令输出
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen failed!");
    }
    // 读取命令输出
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        src += buffer.data();
    }

        // 查找 "80000000" 的位置
    size_t pos = src.find("80000000");

    // 如果找到了 "80000000"
    if (pos != std::string::npos) {
        // 截取从 "80000000" 开始的部分
        src = src.substr(pos);
        // 输出结果
        // std::cout << src << std::endl;
    } else {
        std::cout << "未找到指定字符串" << std::endl;
    }

    return;
}

void Ftrace::getElfHeader() {
    if (fread(&ehdr, 1, sizeof(ehdr), elf_fp) != sizeof(ehdr)) {
        fclose(elf_fp);
        std::cerr << "fread error" << std::endl;
        return;
    }

    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        fclose(elf_fp);
        std::cerr << "Not an ELF file" << std::endl;
        return;
    }
}

void Ftrace::getElfSection() {
    Elf32_Off shoff = ehdr.e_shoff;
    uint16_t shnum = ehdr.e_shnum;
    uint16_t shentsize = ehdr.e_shentsize;
    bool found_symtab = false;
    bool found_strtab = false;

    for (int i = 0; i < shnum; i++) {
        Elf32_Shdr shdr;
        fseek(elf_fp, shoff + i * shentsize, SEEK_SET);

        if (fread(&shdr, 1, sizeof(shdr), elf_fp) != sizeof(shdr)) {
            fclose(elf_fp);
            std::cerr << "fread error" << std::endl;
            return;
        }

        if (shdr.sh_type == SHT_SYMTAB) {
            symtab_shdr = shdr;
            found_symtab = true;
        } else if (shdr.sh_type == SHT_STRTAB && !found_strtab) {
            strtab_shdr = shdr;
            found_strtab = true;
        }

        if (found_symtab && found_strtab)
            break;
    }

    if (!found_symtab || !found_strtab) {
        std::cerr << "Symbol table or string table not found in the ELF file" << std::endl;
        return;
    }
}

void Ftrace::getElfSymbols() {
    std::vector<char> strtab(strtab_shdr.sh_size);

    fseek(elf_fp, strtab_shdr.sh_offset, SEEK_SET);
    if (fread(strtab.data(), 1, strtab_shdr.sh_size, elf_fp) != strtab_shdr.sh_size) {
        std::cerr << "fread strtab error" << std::endl;
        return;
    }

    int num_symbols = symtab_shdr.sh_size / symtab_shdr.sh_entsize;
    Elf32_Sym sym;

    for (int i = 0; i < num_symbols; i++) {
        fseek(elf_fp, symtab_shdr.sh_offset + i * sizeof(sym), SEEK_SET);
        if (fread(&sym, 1, sizeof(sym), elf_fp) != sizeof(sym)) {
            std::cerr << "fread symtab error" << std::endl;
            return;
        }

        if (ELF32_ST_TYPE(sym.st_info) != STT_FUNC)
            continue;

        FuncSym func_sym{};
        strncpy(func_sym.func_name, &strtab[sym.st_name], 20);
        func_sym.func_addr = sym.st_value;
        func_sym.func_size = sym.st_size;
        func_sym_tab.push_back(func_sym);
        func_sym_num++;
    }
}

void Ftrace::parseElfSymbols(const std::string &filename) {
    if(elf_fp == nullptr)
        std::cout << "elf_fp is null" << std::endl;
    if (filename.empty()) {
        std::cerr << "Invalid filename" << std::endl;
        return;
    }

    elf_fp = fopen(filename.c_str(), "rb");
    if (elf_fp == nullptr) {
        std::cerr << "Cannot open ELF file" << std::endl;
        return;
    }

    getElfHeader();
    getElfSection();
    getElfSymbols();

    fclose(elf_fp);
    elf_fp = nullptr;
}
