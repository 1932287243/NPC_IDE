#include "ftrace.h"

Elf32_Ehdr ehdr;
Elf32_Shdr symtab_shdr;
Elf32_Shdr strtab_shdr;

FILE *elf_fp = NULL;
int func_sym_num = 0;
Func_Sym func_sym_tab[40];

void get_elf_header();
void get_elf_section();
void parse_elf_symbols(const char *filename);

void init_ftrace(const char *elf_file) {
  elf_fp = stdout;
  if (elf_file != NULL) {
    FILE *fp = fopen(elf_file, "rb");
    if(fp == NULL)
        printf("Can not open '%s'\n", elf_file);
    elf_fp = fp;
  }
  printf("Ftrace Log is written to %s\n", elf_file ? elf_file : "stdout");
  parse_elf_symbols(elf_file);
}

void get_elf_header()
{
    // 读取ELF头部
    if (fread(&ehdr, 1, sizeof(ehdr), elf_fp) != sizeof(ehdr)) {
        fclose(elf_fp);
        // panic("");
        printf("fread error\n");
        return;
    }
    // 检查魔数是否为ELF文件
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
        ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
        ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        fclose(elf_fp);
        // panic("Not an ELF file");
        printf("Not an ELF file\n");
        return;
    }
}

void get_elf_section()
{
    // 获取 Section Headers 的偏移地址和数量
    Elf32_Off shoff = ehdr.e_shoff;
    uint16_t shnum = ehdr.e_shnum;
    uint16_t shentsize = ehdr.e_shentsize;
    int found_symtab = 0;
    int found_strtab = 0;
    // 遍历读取每个 Section Header
    for (int i = 0; i < shnum; i++) {
        Elf32_Shdr shdr;
        // 移动文件指针到下一个 Section Header
        fseek(elf_fp, shoff + i * shentsize, SEEK_SET);

        // 读取 Section Header
        if (fread(&shdr, 1, sizeof(shdr), elf_fp) != sizeof(shdr)) {
            fclose(elf_fp);
            // panic("fread error");
            printf("fread error\n");
            return;
        }
        if(shdr.sh_type  == SHT_SYMTAB)
        {
            symtab_shdr = shdr;
            found_symtab = 1;
        }  
        else if(shdr.sh_type  == SHT_STRTAB && !found_strtab)
        {
            strtab_shdr = shdr;
            found_strtab = 1;
        }
        if (found_symtab && found_strtab) 
            break; 
    }
    if (!found_symtab || !found_strtab) 
        // panic("Symbol table or string table not found in the ELF file.");
        printf("Symbol table or string table not found in the ELF file.\n");
        return;
}

void get_elf_symbols()
{
    // 读取字符串表
    char *strtab = (char *)malloc(strtab_shdr.sh_size);
    if (!strtab) {
        fclose(elf_fp);
        printf("malloc error\n");
        return;
        // panic("malloc error");
    }
    fseek(elf_fp, strtab_shdr.sh_offset, SEEK_SET);
    if (fread(strtab, 1, strtab_shdr.sh_size, elf_fp) != strtab_shdr.sh_size) {
        free(strtab);
        fclose(elf_fp);
        // panic("fread strtab error");
        printf("fread strtab error\n");
        return;
    }

    // 读取符号表条目
    int num_symbols = symtab_shdr.sh_size / symtab_shdr.sh_entsize;
    Elf32_Sym sym;

    // printf("Symbol Table:\n");
    for (int i = 0; i < num_symbols; i++) {
        fseek(elf_fp, symtab_shdr.sh_offset + i * sizeof(sym), SEEK_SET);
        if (fread(&sym, 1, sizeof(sym), elf_fp) != sizeof(sym)) {
            free(strtab);
            fclose(elf_fp);
            // panic("fread symtab error");
            printf("fread symtab error\n");
            return;
        }
        if(sym.st_info != 18)
            continue;
        
        strncpy(func_sym_tab[func_sym_num].func_name,  &strtab[sym.st_name], 20);
        func_sym_tab[func_sym_num].func_addr = sym.st_value;
        func_sym_tab[func_sym_num].func_size = sym.st_size;
        func_sym_num++;
    }
    free(strtab);
}

void parse_elf_symbols(const char *filename) {
    get_elf_header();
    get_elf_section();
    get_elf_symbols();
    fclose(elf_fp);
}