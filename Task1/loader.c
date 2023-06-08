#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void print_Phdr_address(Elf32_Phdr *program_header, int index);
void print_Phdr_info(Elf32_Phdr *program_header, int index);

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    void *map_start = mmap(NULL, lseek(fd, 0, SEEK_END), PROT_READ, MAP_PRIVATE, fd, 0);
    printf("Type     Offset    VirtAddr  PhysAddr   FileSiz MemSiz  Flg   Align\n");
    foreach_phdr(map_start, print_Phdr_info, -1);

    close(fd);
    return 0;
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{

    Elf32_Ehdr *elfHeader = map_start;
    Elf32_Phdr *programHeaders = map_start + elfHeader->e_phoff;

    for (int i = 0; i < elfHeader->e_phnum; i++)
        func(&programHeaders[i], i);

    return 0;
}

void print_Phdr_address(Elf32_Phdr *program_header, int index)
{
    printf("Program header number %d at address %x\n", index, program_header->p_vaddr);
}

void print_Phdr_info(Elf32_Phdr *program_header, int index)
{
    char *type = program_header->p_type == 0 ? "NULL" : program_header->p_type == 1 ? "LOAD"
                                                    : program_header->p_type == 2   ? "DYNAMIC"
                                                    : program_header->p_type == 3   ? "INTERP"
                                                    : program_header->p_type == 4   ? "NOTE"
                                                    : program_header->p_type == 5   ? "SHLIB"
                                                                                    : "PHDR";
    char *flags = program_header->p_flags == 0x1 ? "E" : program_header->p_flags == 0x2 ? "W"
                                                     : program_header->p_flags == 0x3   ? "W E"
                                                     : program_header->p_flags == 0x4   ? "R"
                                                     : program_header->p_flags == 0x5   ? "R E"
                                                     : program_header->p_flags == 0x6   ? "R W"
                                                                                        : "R W E";
    printf("%-8s %#08x %#010x %#010x %#07x %#07x %-5s %#x\n", type, program_header->p_offset, program_header->p_vaddr, program_header->p_paddr, program_header->p_filesz, program_header->p_memsz, flags, program_header->p_align);
}
