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

int load_shared_library(int* elf_begin_addr)
{
	Elf32_Ehdr* elfhdr = (Elf32_Ehdr*) elf_begin_addr;
	printk("elf loaded in mem at %08x\r\n", elfhdr);
	if (elfhdr->e_ident[0] != '\x7f' || \
		elfhdr->e_ident[1] != 'E' || \
		elfhdr->e_ident[2] != 'L' || \
		elfhdr->e_ident[3] != 'F') {
		printk("elf magic error!\r\n");
		return -1;
	}
	/*if (elfhdr->e_type != 3) {*/
		/*printk("elf type is not shared object!\r\n");*/
		/*return;*/
	/*}*/
	if (elfhdr->e_machine != 243) {
		printk("elf machine is not RISCV!\r\n");
		return -1;
	}

	printk("elf entry addr is %08x(don't care)\r\n", \
			elfhdr->e_entry);
	printk("elf has %d sectors headers, size %d, 0x%d bytes info file\r\n", \
			elfhdr->e_shnum, \
			elfhdr->e_shentsize, \
			elfhdr->e_shoff);

	Elf32_Shdr* elfshdr_base = (Elf32_Shdr *)((unsigned char *)elfhdr + elfhdr->e_shoff);
	Elf32_Shdr* elfshdr_strtab = (Elf32_Shdr *)((Elf32_Shdr *)elfshdr_base + elfhdr->e_shstrndx);

	printk("elfshdr base and strtab: %08x %08x \r\n", \
			elfshdr_base, \
			elfshdr_strtab);

	unsigned char* elfs_strtab = (unsigned char *)elfhdr + elfshdr_strtab->sh_offset;

	printk(".shstrtab section header at %08x, section body at %08x\r\n", \
			elfhdr->e_shoff + elfhdr->e_shstrndx * elfhdr->e_shentsize, \
			elfshdr_strtab->sh_offset);
	printk("Elf32_Shdr struct size %d\r\n", \
			sizeof(Elf32_Shdr));

	int i, j;
	Elf32_Shdr* elfshdr;
	Elf32_Sym* dynsym_start_addr = 0;
	for (i = 0; i < elfhdr->e_shnum; i++) {
		elfshdr = (Elf32_Shdr *)((unsigned char *)elfshdr_base + i * elfhdr->e_shentsize);
		void* sec_start = ((unsigned char *)elfhdr + elfshdr->sh_offset);
		printk("name %20s, addr %08x, size 0x%x\r\n", \
				elfs_strtab + elfshdr->sh_name, \
				elfshdr->sh_addr, \
				elfshdr->sh_size);
		if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".dynsym") == 0) {
			printk("\tfound dynsym section\r\n");
			dynsym_start_addr = (Elf32_Sym *) sec_start;
			/*for (j = 0; j < elfshdr->sh_size/elfshdr->sh_entsize; j++) {*/
				/*printk("%d, %d, %08x\r\n", j, (dynsym_start_addr+j)->st_size, (dynsym_start_addr+j)->st_value);*/
			/*}*/
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.dyn") == 0) {
			printk("\tfound rela.dyn section\r\n");
			if (dynsym_start_addr == 0) {
				printk("\tdynsym section should already found!\r\n");
				return -1;
			}
			Elf32_Rela* reladyn_start_addr = (Elf32_Sym *) sec_start;
			for (j = 0; j < elfshdr->sh_size/elfshdr->sh_entsize; j++) {
				Elf32_Rela* reladyn_entry = reladyn_start_addr + j;
				int got_offset = reladyn_entry->r_offset;
				int r_info = reladyn_entry->r_info;
				if (r_info % 0x100 != 1) {
					printk("rela.plt entry is not R_RISCV_32!\r\n");
					continue;
				}
				int dynsym_idx = r_info / 0x100;
				unsigned int* got_entry_addr = (unsigned char *)elf_begin_addr + got_offset;
				unsigned int symval = (dynsym_start_addr+dynsym_idx)->st_value;
				printk("\t\t\t %08x %08x %6d %8x\r\n", got_offset, r_info, dynsym_idx, symval);
				*got_entry_addr = (unsigned int)elf_begin_addr + symval;
			}
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.plt") == 0) {
			printk("\tfound rela.plt section\r\n");
			if (dynsym_start_addr == 0) {
				printk("\tdynsym section should already found!\r\n");
				return -1;
			}
			printk("\t\tsize %x\r\n", elfshdr->sh_size);
			printk("\t\tentry size %x\r\n", elfshdr->sh_entsize);
			printk("\t\tElf32_Rel size %x\r\n", sizeof(Elf32_Rela));
			Elf32_Rela* relaplt_start_addr = (Elf32_Rela *) sec_start;
			printk("\t\t\t %s %s %s %s\r\n", "ofst@got", "info", "symidx", "symval");
			for (j = 0; j < elfshdr->sh_size/elfshdr->sh_entsize; j++) {
				Elf32_Rela* relaplt_entry = relaplt_start_addr + j;
				int got_offset = relaplt_entry->r_offset;
				int r_info = relaplt_entry->r_info;
				if (r_info % 0x100 != 5) {
					printk("rela.plt entry is not R_RISCV_JUMP_SLOT!\r\n");
					continue;
				}
				int dynsym_idx = r_info / 0x100;
				unsigned int* got_entry_addr = (unsigned char *)elf_begin_addr + got_offset;
				unsigned int symval = (dynsym_start_addr+dynsym_idx)->st_value;
				/*printk("\t\t\t %08x %08x %6d %8x\r\n", got_offset, r_info, dynsym_idx, symval);*/
				*got_entry_addr = (unsigned int)elf_begin_addr + symval;
			}
			printk("\t\t%d rela.plt entry processed\r\n", j);
		}
	}
	return 0;
}

int main()
{
	/*FILE* fin = fopen("../userspace/test", "r");*/
	FILE* fin = fopen("../userspace/libc.so", "r");
	int* elf_file = (int*)malloc(200000*sizeof(int));
	fread(elf_file, sizeof(int), 200000, fin);

	/*elf_header_check(elf_file);*/
	load_shared_library(elf_file);

	fclose(fin);
	FILE* fout = fopen("../userspace/libc.patched.so", "wb");
	fwrite(elf_file, sizeof(int), 200000, fout);
	fclose(fout);
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
