/**
 * File              : elf.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.06.09
 * Last Modified Date: 2021.06.09
 */
#include "elf.h"
/*#include "global.h"*/
/*#include "misc.h"*/
#include "stdio.h"
#define printk printf

int elf_header_check(int* elf_begin_addr)
{
	void* elf_begin_addr_void = (void*)elf_begin_addr;
	Elf32_Ehdr* elfhdr = (Elf32_Ehdr *)elf_begin_addr_void;
	if (elfhdr->e_ident[0] != '\x7f' || \
		elfhdr->e_ident[1] != 'E' || \
		elfhdr->e_ident[2] != 'L' || \
		elfhdr->e_ident[3] != 'F') {
		printk("elf magic error!\r\n");
		return 0;
	}
	if (elfhdr->e_type != 2) {
		printk("elf type is not executable!\r\n");
		return 0;
	}
	if (elfhdr->e_machine != 243) {
		printk("elf machine is not RISCV!\r\n");
		return 0;
	}
	printk("elf entry addr is %08x(should be 0x0 for my relocation)\r\n", elfhdr->e_entry);
	printk("elf has %d program headers but don't care\r\n", elfhdr->e_phnum);
	printk("elf has %d sectors headers, size %d, %d bytes info file\r\n", elfhdr->e_shnum, elfhdr->e_shentsize, elfhdr->e_shoff);
	Elf32_Shdr* elfshdr_base = (Elf32_Shdr *)((unsigned char *)elfhdr + elfhdr->e_shoff);
	printk("Elf32_Shdr struct size %d\r\n", sizeof(Elf32_Shdr));
	int i;
	for (i = 0; i < elfhdr->e_shnum; i++) {
		Elf32_Shdr* elfshdr = (Elf32_Shdr *)((unsigned char *)elfshdr_base + i * elfhdr->e_shentsize);
		printk("name %08x, addr %08x\r\n", elfshdr->sh_name, elfshdr->sh_addr);
	}
	printk("%08x, %08x\r\n", elfhdr, elfshdr_base);
	printk("%08x\r\n", elfhdr->e_shstrndx);


	return 1;
}

int main()
{
	FILE* fin = fopen("../test", "r");
	int* elf_file = (int*)malloc(40000*sizeof(int));
	fread(elf_file, sizeof(int), 40000, fin);
	elf_header_check(elf_file);
	fclose(fin);
	return 0;
}
