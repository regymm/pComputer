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

int elf_strcmp(const char* a, const char* b)
{
	int i = 0;
	while (a[i] == b[i] && a[i] && b[i]) i++;
	return !(a[i] == 0 && b[i] == 0);
}

// this should cooperate with my custom userspace linker script
void elf_header_check(int* elf_begin_addr, int stack_size, unsigned int** entry_addr, unsigned int** stack_addr)
{
	Elf32_Ehdr* elfhdr = (Elf32_Ehdr *)elf_begin_addr;
	printk("elf loaded in mem at %08x\r\n", elfhdr);
	if (elfhdr->e_ident[0] != '\x7f' || \
		elfhdr->e_ident[1] != 'E' || \
		elfhdr->e_ident[2] != 'L' || \
		elfhdr->e_ident[3] != 'F') {
		printk("elf magic error!\r\n");
		*entry_addr = 0; return;
	}
	if (elfhdr->e_type != 2) {
		printk("elf type is not executable!\r\n");
		*entry_addr = 0; return;
	}
	if (elfhdr->e_machine != 243) {
		printk("elf machine is not RISCV!\r\n");
		*entry_addr = 0; return;
	}
	printk("elf entry addr is %08x(should be 0x0 for my relocation)\r\n", \
			elfhdr->e_entry);
	printk("elf has %d program headers but don't care\r\n", \
			elfhdr->e_phnum);
	printk("elf has %d sectors headers, size %d, %d bytes info file\r\n", \
			elfhdr->e_shnum, elfhdr->e_shentsize, elfhdr->e_shoff);
	Elf32_Shdr* elfshdr_base = (Elf32_Shdr *)((unsigned char *)elfhdr + elfhdr->e_shoff);
	Elf32_Shdr* elfshdr_strtab = (Elf32_Shdr *)((Elf32_Shdr *)elfshdr_base + elfhdr->e_shstrndx);
	printk("elfshdr base and strtab: %08x %08x \r\n", \
			elfshdr_base, elfshdr_strtab);
	unsigned char* elfs_strtab = (unsigned char *)elfhdr + elfshdr_strtab->sh_offset;
	printk(".shstrtab section header at %08x, section body at %08x\r\n", \
			elfhdr->e_shoff + elfhdr->e_shstrndx * elfhdr->e_shentsize, elfshdr_strtab->sh_offset);
	printk("Elf32_Shdr struct size %d\r\n", \
			sizeof(Elf32_Shdr));

	unsigned int runtime_entry_addr = 0xffffffff;
	unsigned int default_link_addr = 0x12340000;

	int i;
	int j;
	Elf32_Shdr* elfshdr;
	for (i = 0; i < elfhdr->e_shnum; i++) {
		elfshdr = (Elf32_Shdr *)((unsigned char *)elfshdr_base + i * elfhdr->e_shentsize);
		printk("name %s\t, addr %08x, size 0x%x\r\n", \
				elfs_strtab + elfshdr->sh_name, elfshdr->sh_addr, elfshdr->sh_size);
		if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".text") == 0) {
			printk("	found text section\r\n");
			runtime_entry_addr = \
				(unsigned int) \
				((unsigned char *)elfhdr + elfshdr->sh_offset);
			printk("	entry address in mem is  %08x\r\n", runtime_entry_addr);
		}
		if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".data") == 0)
			printk("	found data section\r\n");
		if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".got") == 0) {
			printk("	found got section\r\n");
			for (j = 0; j < elfshdr->sh_size / 4; j++) {
				unsigned int* got_entry_addr = \
					(unsigned int*) \
					((unsigned char *)elfhdr + elfshdr->sh_offset + j*4);
				printk("%08x -> ", *got_entry_addr);
				// TODO: fix this crude judgement
				if (*got_entry_addr != 0xffffffff && \
						*got_entry_addr > default_link_addr) {
					*got_entry_addr -= default_link_addr;
					*got_entry_addr += runtime_entry_addr;
				}
				printk("%08x\r\n", *got_entry_addr);
			}
			printk("	got patched up\r\n");
		}
		// TODO: got.blt
		if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".bss") == 0) {
			printk("	found .bss section\r\n");
			// at last is .bss, clear it and let stack after it
			// now most elf data may be override
			unsigned int* bss_entry_addr = \
				(unsigned int*) \
				((unsigned char *)elfhdr + elfshdr->sh_offset);
			for (j = 0; j < elfshdr->sh_size / 4; j++)
				bss_entry_addr[j] = 0;
			printk("	.bss cleared\r\n");
			if (stack_size > 0) {
				*stack_addr = bss_entry_addr + j + stack_size / 4;
				printk("	stack calculated: %x \r\n", *stack_addr);
			}
			else printk("	no stack size specified\r\n");
			printk("return\r\n");
			/*return (unsigned int*)runtime_entry_addr;*/
			*entry_addr = (unsigned int*)runtime_entry_addr;
			return;
		}
	}
	*entry_addr = 0; return;
}

int main()
{
	FILE* fin = fopen("../userspace/test", "r");
	int* elf_file = (int*)malloc(40000*sizeof(int));
	fread(elf_file, sizeof(int), 40000, fin);
	/*elf_header_check(elf_file);*/
	fclose(fin);
	unsigned int main2 = 0x20021000;
	putchar(((int)main2 >> 28) + '0');
	putchar(((int)main2 >> 24) + '0');
	putchar(((int)main2 >> 20) + '0');
	putchar(((int)main2 >> 16) + '0');
	putchar(((int)main2 >> 12) + '0');
	putchar(((int)main2 >> 8) + '0');
	putchar(((int)main2 >> 4) + '0');
	putchar(((int)main2 >> 0) + '0');
	return 0;
}
