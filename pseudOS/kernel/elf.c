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

/*extern SOSearch_Metadata libcso_data;*/
extern DynLinkTable dynlinktbl;

int elf_strcmp(const char* a, const char* b)
{
	int i = 0;
	while (a[i] == b[i] && a[i] && b[i]) i++;
	return !(a[i] == 0 && b[i] == 0);
}

void elf_strcpy(char* dest, const char* src)
{
	int i = 0;
	while ((dest[i] = src[i])) i++;
}

/*// this should cooperate with my custom userspace linker script*/
/*void elf_header_check(int* elf_begin_addr, int stack_size, unsigned int** entry_addr, unsigned int** stack_addr)*/
/*{*/
	/*Elf32_Ehdr* elfhdr = (Elf32_Ehdr *)elf_begin_addr;*/
	/*printk("elf loaded in mem at %08x\r\n", elfhdr);*/
	/*if (elfhdr->e_ident[0] != '\x7f' || \*/
		/*elfhdr->e_ident[1] != 'E' || \*/
		/*elfhdr->e_ident[2] != 'L' || \*/
		/*elfhdr->e_ident[3] != 'F') {*/
		/*printk("elf magic error!\r\n");*/
		/**entry_addr = 0; return;*/
	/*}*/
	/*if (elfhdr->e_type != 2) {*/
		/*printk("elf type is not executable!\r\n");*/
		/**entry_addr = 0; return;*/
	/*}*/
	/*if (elfhdr->e_machine != 243) {*/
		/*printk("elf machine is not RISCV!\r\n");*/
		/**entry_addr = 0; return;*/
	/*}*/
	/*printk("elf entry addr is %08x(should be 0x0 for my relocation)\r\n", \*/
			/*elfhdr->e_entry);*/
	/*printk("elf has %d program headers but don't care\r\n", \*/
			/*elfhdr->e_phnum);*/
	/*printk("elf has %d sectors headers, size %d, %d bytes info file\r\n", \*/
			/*elfhdr->e_shnum, elfhdr->e_shentsize, elfhdr->e_shoff);*/
	/*Elf32_Shdr* elfshdr_base = (Elf32_Shdr *)((unsigned char *)elfhdr + elfhdr->e_shoff);*/
	/*Elf32_Shdr* elfshdr_strtab = (Elf32_Shdr *)((Elf32_Shdr *)elfshdr_base + elfhdr->e_shstrndx);*/
	/*printk("elfshdr base and strtab: %08x %08x \r\n", \*/
			/*elfshdr_base, elfshdr_strtab);*/
	/*unsigned char* elfs_strtab = (unsigned char *)elfhdr + elfshdr_strtab->sh_offset;*/
	/*printk(".shstrtab section header at %08x, section body at %08x\r\n", \*/
			/*elfhdr->e_shoff + elfhdr->e_shstrndx * elfhdr->e_shentsize, elfshdr_strtab->sh_offset);*/
	/*printk("Elf32_Shdr struct size %d\r\n", \*/
			/*sizeof(Elf32_Shdr));*/

	/*unsigned int runtime_entry_addr = 0xffffffff;*/
	/*unsigned int default_link_addr = 0x12340000;*/

	/*int i;*/
	/*int j;*/
	/*Elf32_Shdr* elfshdr;*/
	/*for (i = 0; i < elfhdr->e_shnum; i++) {*/
		/*elfshdr = (Elf32_Shdr *)((unsigned char *)elfshdr_base + i * elfhdr->e_shentsize);*/
		/*printk("name %s\t, addr %08x, size 0x%x\r\n", \*/
				/*elfs_strtab + elfshdr->sh_name, elfshdr->sh_addr, elfshdr->sh_size);*/
		/*if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".text") == 0) {*/
			/*printk("	found text section\r\n");*/
			/*runtime_entry_addr = \*/
				/*(unsigned int) \*/
				/*((unsigned char *)elfhdr + elfshdr->sh_offset);*/
			/*printk("	entry address in mem is  %08x\r\n", runtime_entry_addr);*/
		/*}*/
		/*if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".data") == 0)*/
			/*printk("	found data section\r\n");*/
		/*if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".got") == 0) {*/
			/*printk("	found got section\r\n");*/
			/*for (j = 0; j < elfshdr->sh_size / 4; j++) {*/
				/*unsigned int* got_entry_addr = \*/
					/*(unsigned int*) \*/
					/*((unsigned char *)elfhdr + elfshdr->sh_offset + j*4);*/
				/*printk("%08x -> ", *got_entry_addr);*/
				/*// TODO: fix this crude judgement*/
				/*if (*got_entry_addr != 0xffffffff && \*/
						/**got_entry_addr > default_link_addr) {*/
					/**got_entry_addr -= default_link_addr;*/
					/**got_entry_addr += runtime_entry_addr;*/
				/*}*/
				/*printk("%08x\r\n", *got_entry_addr);*/
			/*}*/
			/*printk("	got patched up\r\n");*/
		/*}*/
		/*// TODO: got.blt*/
		/*if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".bss") == 0) {*/
			/*printk("	found .bss section\r\n");*/
			/*// at last is .bss, clear it and let stack after it*/
			/*// now most elf data may be override*/
			/*unsigned int* bss_entry_addr = \*/
				/*(unsigned int*) \*/
				/*((unsigned char *)elfhdr + elfshdr->sh_offset);*/
			/*for (j = 0; j < elfshdr->sh_size / 4; j++)*/
				/*bss_entry_addr[j] = 0;*/
			/*printk("	.bss cleared\r\n");*/
			/*if (stack_size > 0) {*/
				/**stack_addr = bss_entry_addr + j + stack_size / 4;*/
				/*printk("	stack calculated: %x \r\n", *stack_addr);*/
			/*}*/
			/*else printk("	no stack size specified\r\n");*/
			/*printk("return\r\n");*/
			/*[>return (unsigned int*)runtime_entry_addr;<]*/
			/**entry_addr = (unsigned int*)runtime_entry_addr;*/
			/*return;*/
		/*}*/
	/*}*/
	/**entry_addr = 0; return;*/
/*}*/

int find_function_from_so(const char* fname)
{
	int i;
	for (i = 0; i < dynlinktbl.size; i++) 
		if (elf_strcmp(fname, dynlinktbl.tbl[i].name) == 0)
			return dynlinktbl.tbl[i].addr;
	printk("function not found in libc.so!\r\n");
	return -1;
}

int load_shared_library(int* elf_begin_addr)
{
	int i, j;
	Elf32_Ehdr* elfhdr = (Elf32_Ehdr*) elf_begin_addr;
	printk("elf loaded in mem at %08x\r\n", elfhdr);
	if (elfhdr->e_ident[0] != '\x7f' || \
		elfhdr->e_ident[1] != 'E' || \
		elfhdr->e_ident[2] != 'L' || \
		elfhdr->e_ident[3] != 'F') {
		printk("elf magic error!\r\n");
		return -1;
	}
	if (elfhdr->e_type != 3) {
		printk("elf type is not shared object!\r\n");
		return -1;
	}
	if (elfhdr->e_machine != 243) {
		printk("elf machine is not RISCV!\r\n");
		return -1;
	}

	printk("elf entry addr is %08x(don't care)\r\n", \
			elfhdr->e_entry);
	printk("elf has %d program headers, %d sectors headers, size %d, 0x%d bytes info file\r\n", \
			elfhdr->e_phnum, \
			elfhdr->e_shnum, \
			elfhdr->e_shentsize, \
			elfhdr->e_shoff);

	Elf32_Phdr* elfphdr_base = (Elf32_Phdr *)((unsigned char*)elfhdr + elfhdr->e_phoff);
	Elf32_Phdr* elfphdr_first = elfphdr_base;
	if (!(elfphdr_first->p_type == PT_LOAD && \
			elfphdr_first->p_offset == 0 && \
			elfphdr_first->p_vaddr == 0 && \
			elfphdr_first->p_paddr == 0)) {
		printk("First program header is not LOAD 0x0 0x0 0x0, panic\r\n");
		return -1;
	}
	Elf32_Phdr* elfphdr_second = elfphdr_base + 1;
	if (!(elfphdr_second->p_type == PT_LOAD)) {
		printk("Second program header is not LOAD, panic\r\n");
		return -1;
	}
	if (!(elfphdr_second->p_vaddr == elfphdr_second->p_paddr)) {
		printk("Second program header PA != VA, panic\r\n");
		return -1;
	}
	if (!(elfphdr_second->p_offset <= elfphdr_second->p_vaddr)) {
		printk("Second program header offset > VA, panic\r\n");
		return -1;
	}
	int load2_offset = elfphdr_second->p_offset;
	int load2_addr = elfphdr_second->p_vaddr;
	int load2_filesize = elfphdr_second->p_filesz;
	int load2_memsize = elfphdr_second->p_memsz;
	int load2_inmen_offset = load2_addr - load2_offset;
	printk("Ignore other program headers\r\n");
	/*for (i = 0; i < elfhdr->e_shnum; i++) {*/
		/*Elf32_Phdr* elfphdr = elfphdr_base + i;*/
		/*if (elfphdr->p_type != PT_LOAD) {*/
			/*printk("unknown program header %x, ignore\r\n", elfphdr->p_type);*/
			/*continue;*/
		/*}*/
	/*}*/


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

	Elf32_Shdr* elfshdr;
	Elf32_Shdr* dynsym_shdr = 0;
	Elf32_Shdr* reladyn_shdr = 0;
	Elf32_Shdr* relaplt_shdr = 0;
	Elf32_Sym* dynsym_start_addr = 0;
	char* dynstr_start_addr = 0;
	Elf32_Rela* reladyn_start_addr = 0;
	Elf32_Rela* relaplt_start_addr = 0;
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
			dynsym_shdr = elfshdr;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".dynstr") == 0) {
			printk("\tfound dynstr section\r\n");
			dynstr_start_addr = (char *) sec_start;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.dyn") == 0) {
			printk("\tfound rela.dyn section\r\n");
			reladyn_start_addr = (Elf32_Rela *) sec_start;
			reladyn_shdr = elfshdr;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.plt") == 0) {
			printk("\tfound rela.plt section\r\n");
			relaplt_start_addr = (Elf32_Rela *) sec_start;
			relaplt_shdr = elfshdr;
		}
	}

	if (!((unsigned int)dynsym_start_addr & \
				(unsigned int)reladyn_start_addr & \
				(unsigned int)relaplt_start_addr)) {
		printk("\tlacking section!\r\n");
		return -1;
	}

	// filling up dynlinktable
	printk("fill up DynLinkTable\r\n");
	int dynsym_entrycnt = dynsym_shdr->sh_size/dynsym_shdr->sh_entsize;
	dynlinktbl.size = dynsym_entrycnt;
	for (j = 0; j < dynsym_entrycnt; j++) {
		Elf32_Sym* entry = dynsym_start_addr + j;
		/*printk("%d, %d, %08x, %d, %s\r\n", j, entry->st_size, entry->st_value, entry->st_name, dynstr_start_addr + entry->st_name);*/
		dynlinktbl.tbl[j].addr = (unsigned int)elf_begin_addr + entry->st_value;
		elf_strcpy(dynlinktbl.tbl[j].name, dynstr_start_addr + entry->st_name);
	}
	printk("\t\t%d dynsym entries processed\r\n", j);
	
	// rela.dyn patching
	printk("\t\t\t %s %s %s %s %s\r\n", "ofst@got", "info", "symidx", "symval", "addend");
	for (j = 0; j < reladyn_shdr->sh_size/reladyn_shdr->sh_entsize; j++) {
		Elf32_Rela* reladyn_entry = reladyn_start_addr + j;
		int got_offset = reladyn_entry->r_offset - load2_inmen_offset; // maybe not got actually
		int r_info = reladyn_entry->r_info;
		int r_addend = reladyn_entry->r_addend;
		/*if (r_info % 0x100 != 1) {*/
			/*printk("rela.dyn entry is not R_RISCV_32!\r\n");*/
			/*[>continue;<]*/
		/*}*/
		int dynsym_idx = r_info / 0x100;
		unsigned int* got_entry_addr = (unsigned char *)elf_begin_addr + got_offset;
		unsigned int symval = (dynsym_start_addr+dynsym_idx)->st_value;
		/*printk("\t\t\t %08x %08x %6d %8x %8x\r\n", got_offset, r_info, dynsym_idx, symval, r_addend);*/
		*got_entry_addr = (unsigned int)elf_begin_addr + symval + r_addend;
	}
	printk("\t\t%d rela.dyn entries processed\r\n", j);

	// rela.plt patching
	printk("\t\tsize %x\r\n", relaplt_shdr->sh_size);
	printk("\t\tentry size %x\r\n", relaplt_shdr->sh_entsize);
	printk("\t\tElf32_Rel size %x\r\n", sizeof(Elf32_Rela));
	printk("\t\t\t %s %s %s %s\r\n", "ofst@got", "info", "symidx", "symval");
	for (j = 0; j < relaplt_shdr->sh_size/relaplt_shdr->sh_entsize; j++) {
		Elf32_Rela* relaplt_entry = relaplt_start_addr + j;
		int got_offset = relaplt_entry->r_offset - load2_inmen_offset;
		int r_info = relaplt_entry->r_info;
		if (r_info % 0x100 != 5) {
			printk("rela.plt entry is not R_RISCV_JUMP_SLOT!\r\n");
			return -1;
		}
		int dynsym_idx = r_info / 0x100;
		unsigned int* got_entry_addr = (unsigned char *)elf_begin_addr + got_offset;
		unsigned int symval = (dynsym_start_addr+dynsym_idx)->st_value;
		/*printk("\t\t\t %08x %08x %6d %8x\r\n", got_offset, r_info, dynsym_idx, symval);*/
		*got_entry_addr = (unsigned int)elf_begin_addr + symval;
	}
	printk("\t\t%d rela.plt entry processed\r\n", j);

	// second LOAD processing
	for (i = load2_memsize - load2_filesize - 4; i >= 0; i-=4)
		elf_begin_addr[(load2_addr + load2_memsize - load2_filesize + i)/4] = 0;
	for (i = load2_filesize - 4; i >= 0; i-=4)
		elf_begin_addr[(load2_addr + i)/4] = elf_begin_addr[(load2_offset + i)/4];
	
	// now shstrtab and section headers are already corrupted

	return 0;
}

// elf begin addr in memory
// stack size
// entry addr which .text starts used to set $pc
// stack addr(top of stack) used to set $sp
// TODO: use stack_size?
int load_dynamic_exec(int* elf_begin_addr, int stack_size, int** entry_addr, int** stack_addr)
{
	// basic processing, just like libc.so
	Elf32_Ehdr* elfhdr = (Elf32_Ehdr *)elf_begin_addr;
	printk("elf loaded in mem at %08x\r\n", elfhdr);
	if (elfhdr->e_ident[0] != '\x7f' || \
		elfhdr->e_ident[1] != 'E' || \
		elfhdr->e_ident[2] != 'L' || \
		elfhdr->e_ident[3] != 'F') {
		printk("elf magic error!\r\n");
		*entry_addr = 0; return -1;
	}
	if (elfhdr->e_type != 3) {
		printk("elf type is not dyn!\r\n");
		*entry_addr = 0; return -1;
	}
	if (elfhdr->e_machine != 243) {
		printk("elf machine is not RISCV!\r\n");
		*entry_addr = 0; return -1;
	}

	printk("elf entry addr is %08x\r\n", \
			elfhdr->e_entry);
	*entry_addr = (int*)((int)elfhdr->e_entry + (int)elfhdr);

	printk("elf has %d program headers, \
			%d sectors headers, \
			size %d, \
			0x%d bytes info file\r\n", \
			elfhdr->e_phnum, \
			elfhdr->e_shnum, \
			elfhdr->e_shentsize, \
			elfhdr->e_shoff);

	Elf32_Phdr* elfphdr_base = (Elf32_Phdr *)((unsigned char*)elfhdr + elfhdr->e_phoff);
	Elf32_Phdr* elfphdr_first = elfphdr_base + 2; // TODO: remove hard code, do the analysis!
	if (!(elfphdr_first->p_type == PT_LOAD && \
			elfphdr_first->p_offset == 0 && \
			elfphdr_first->p_vaddr == 0 && \
			elfphdr_first->p_paddr == 0)) {
		printk("First program header is not LOAD 0x0 0x0 0x0, panic\r\n");
		return -1;
	}
	Elf32_Phdr* elfphdr_second = elfphdr_base + 3;
	if (!(elfphdr_second->p_type == PT_LOAD)) {
		printk("Second program header is not LOAD, panic\r\n");
		return -1;
	}
	if (!(elfphdr_second->p_vaddr == elfphdr_second->p_paddr)) {
		printk("Second program header PA != VA, panic\r\n");
		return -1;
	}
	if (!(elfphdr_second->p_offset <= elfphdr_second->p_vaddr)) {
		printk("Second program header offset > VA, panic\r\n");
		return -1;
	}
	int load2_offset = elfphdr_second->p_offset;
	int load2_addr = elfphdr_second->p_vaddr;
	int load2_filesize = elfphdr_second->p_filesz;
	int load2_memsize = elfphdr_second->p_memsz;
	printk("Ignore other program headers\r\n");

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
	Elf32_Shdr* dynsym_shdr = 0;
	Elf32_Shdr* reladyn_shdr = 0;
	Elf32_Shdr* relaplt_shdr = 0;
	Elf32_Sym* dynsym_start_addr = 0;
	char* dynstr_start_addr = 0;
	Elf32_Rela* reladyn_start_addr = 0;
	Elf32_Rela* relaplt_start_addr = 0;
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
			dynsym_shdr = elfshdr;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".dynstr") == 0) {
			printk("\tfound dynstr section\r\n");
			dynstr_start_addr = (char *) sec_start;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.dyn") == 0) {
			printk("\tfound rela.dyn section\r\n");
			reladyn_start_addr = (Elf32_Rela *) sec_start;
			reladyn_shdr = elfshdr;
		}
		else if (elf_strcmp(elfs_strtab + elfshdr->sh_name, ".rela.plt") == 0) {
			printk("\tfound rela.plt section\r\n");
			relaplt_start_addr = (Elf32_Rela *) sec_start;
			relaplt_shdr = elfshdr;
		}
	}

	// collect dynlinktable for symbol-finding after second LOAD override
	printk("collect dynamic link entries from rela.plt\r\n");
	DynLinkTable temptbl;
	int relaplt_entrycnt = relaplt_shdr->sh_size/relaplt_shdr->sh_entsize;
	temptbl.size = relaplt_entrycnt;
	for (j = 0; j < relaplt_entrycnt; j++) {
		Elf32_Rela* relaplt_entry = relaplt_start_addr + j;
		int r_info = relaplt_entry->r_info;
		if (r_info % 0x100 != 5) {
			printk("rela.plt entry is not R_RISCV_JUMP_SLOT!\r\n");
			return -1;
		}
		int dynsym_idx = r_info / 0x100;
		Elf32_Sym* entry = dynsym_start_addr + dynsym_idx; // dynsym entry
		printk("%d, %d, %08x, %d, %s\r\n", j, entry->st_size, entry->st_value, entry->st_name, dynstr_start_addr + entry->st_name);
		temptbl.tbl[j].addr = (unsigned int)elf_begin_addr + relaplt_entry->r_offset;
		elf_strcpy(temptbl.tbl[j].name, dynstr_start_addr + entry->st_name);
	}

	// second LOAD processing
	for (i = load2_memsize - load2_filesize - 4; i >= 0; i-=4)
		elf_begin_addr[(load2_addr + load2_memsize - load2_filesize + i)/4] = 0;
	for (i = load2_filesize - 4; i >= 0; i-=4)
		elf_begin_addr[(load2_addr + i)/4] = elf_begin_addr[(load2_offset + i)/4];
	// now shstrtab and section headers are already corrupted
	// but relocation offsets are real in-mem addr
	
	// stack bottom is top of the LOAD
	*stack_addr = (int *)((int)elf_begin_addr + load2_addr + load2_memsize);
	
	// we can do dynamic linking now
	for (j = 0; j < temptbl.size; j++) {
		int* dyn_loc_in_mem = (int *)temptbl.tbl[j].addr;
		int dyn_sym_addr = find_function_from_so(temptbl.tbl[j].name);
		*dyn_loc_in_mem = dyn_sym_addr;
		printk("%08x <- %08x\r\n", dyn_loc_in_mem, *dyn_loc_in_mem);
	}

	// symbol relocation: rela.dyn patching
	printk("\t\t\t %s %s %s %s %s\r\n", "ofst@got", "info", "symidx", "symval", "addend");
	for (j = 0; j < reladyn_shdr->sh_size/reladyn_shdr->sh_entsize; j++) {
		Elf32_Rela* reladyn_entry = reladyn_start_addr + j;
		int got_offset = reladyn_entry->r_offset; // maybe not got actually
		int r_info = reladyn_entry->r_info;
		int r_addend = reladyn_entry->r_addend;
		/*if (r_info % 0x100 != 1) {*/
			/*printk("rela.dyn entry is not R_RISCV_32!\r\n");*/
			/*continue;*/
		/*}*/
		int dynsym_idx = r_info / 0x100;
		unsigned int* got_entry_addr = (unsigned char *)elf_begin_addr + got_offset;
		unsigned int symval = (dynsym_start_addr+dynsym_idx)->st_value;
		printk("\t\t\t %08x %08x %6d %8x %8x\r\n", got_offset, r_info, dynsym_idx, symval, r_addend);
		*got_entry_addr = (unsigned int)elf_begin_addr + symval + r_addend;
	}

	return 0;
}

int main()
{
	/*FILE* fin = fopen("../userspace/test", "r");*/
	FILE* fin = fopen("../userspace/libc.so", "r");
	int* elf_file = (int*)malloc(200000*sizeof(int));
	fread(elf_file, sizeof(int), 200000, fin);
	fclose(fin);

	/*elf_header_check(elf_file);*/
	load_shared_library(elf_file);
	printk("%08x\r\n", find_function_from_so("strcpy"));

	FILE* fin2 = fopen("../userspace/hello", "r");
	int* elf_exec = (int*)malloc(200000*sizeof(int));
	fread(elf_exec, sizeof(int), 200000, fin2);
	fclose(fin2);

	int* entry_addr = 0;
	int* stack_addr = 0;
	load_dynamic_exec(elf_exec, 0x10000, &entry_addr, &stack_addr);
	printk("%08x, %08x\r\n", entry_addr, stack_addr);

	/*FILE* fout = fopen("../userspace/libc.patched.so", "wb");*/
	/*fwrite(elf_file, sizeof(int), 200000, fout);*/
	/*fclose(fout);*/
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
