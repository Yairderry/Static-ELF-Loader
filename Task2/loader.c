#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void load_phdr(Elf32_Phdr *phdr, int fd);
void print_Phdr_info(Elf32_Phdr *program_header, int index);
int extern startup(int argc, char **argv, void (*start)());

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    void *map_start = mmap(NULL, lseek(fd, 0, SEEK_END), PROT_READ, MAP_PRIVATE, fd, 0);
    Elf32_Ehdr *elf_header = map_start;

    printf("Type     Offset    VirtAddr  PhysAddr   FileSiz MemSiz  Flg   Align ProtFlg MapFlg\n");
    foreach_phdr(map_start, load_phdr, fd);

    startup(argc - 1, argv + 1, (void *)(elf_header->e_entry));

    close(fd);
    return 0;
}

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg)
{

    Elf32_Ehdr *elfHeader = map_start;
    Elf32_Phdr *programHeaders = map_start + elfHeader->e_phoff;

    for (int i = 0; i < elfHeader->e_phnum; i++)
        func(&programHeaders[i], arg);

    return 0;
}

void load_phdr(Elf32_Phdr *phdr, int fd)
{
    print_Phdr_info(phdr, fd);

    if (phdr->p_type != PT_LOAD)
        return;

    int proc_flags = phdr->p_flags == PF_X ? PROT_EXEC : phdr->p_flags == PF_W      ? PROT_WRITE
                                                     : phdr->p_flags == PF_R        ? PROT_READ
                                                     : phdr->p_flags == PF_X | PF_W ? PROT_EXEC | PROT_WRITE
                                                     : phdr->p_flags == PF_X | PF_R ? PROT_EXEC | PROT_READ
                                                     : phdr->p_flags == PF_W | PF_R ? PROT_READ | PROT_WRITE
                                                                                    : PROT_EXEC | PROT_READ | PROT_WRITE;
    int vaddr = phdr->p_vaddr & 0xfffff000;
    int offset = phdr->p_offset & 0xfffff000;
    int padding = phdr->p_vaddr & 0xfff;
    int *map = mmap((void *)vaddr, phdr->p_memsz + padding, proc_flags, MAP_PRIVATE | MAP_FIXED, fd, offset);
}

void print_Phdr_info(Elf32_Phdr *program_header, int _)
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

    char *prot_flags = program_header->p_type != 1 ? "" : program_header->p_flags == 0x1 ? "PROC_EXEC"
                                                      : program_header->p_flags == 0x2   ? "PROT_WRITE"
                                                      : program_header->p_flags == 0x3   ? "PROT_WRITE PROC_EXEC"
                                                      : program_header->p_flags == 0x4   ? "PROT_READ"
                                                      : program_header->p_flags == 0x5   ? "PROT_READ PROC_EXEC"
                                                      : program_header->p_flags == 0x6   ? "PROT_READ PROT_WRITE"
                                                                                         : "PROT_READ PROT_WRITE PROC_EXEC";
    char *map_flags = program_header->p_type != 1 ? "" : "MAP_PRIVATE";

    printf("%-8s %#08x %#010x %#010x %#07x %#07x %-5s %#06x %s %s\n", type, program_header->p_offset, program_header->p_vaddr, program_header->p_paddr, program_header->p_filesz, program_header->p_memsz, flags, program_header->p_align, prot_flags, map_flags);
}
