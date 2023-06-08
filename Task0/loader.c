#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

int foreach_phdr(void *map_start, void (*func)(Elf32_Phdr *, int), int arg);
void print_Phdr_address(Elf32_Phdr *program_header, int index);

int main(int argc, char *argv[])
{
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        fprintf(stderr, "Couldn't open file %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    void *map_start = mmap(NULL, lseek(fd, 0, SEEK_END), PROT_READ, MAP_PRIVATE, fd, 0);
    foreach_phdr(map_start, print_Phdr_address, -1);

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
