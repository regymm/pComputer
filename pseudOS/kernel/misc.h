/**
 * File              : misc.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.06
 * Last Modified Date: 2021.09.21
 */
// misc *functions* required for kernel
#ifndef PSEUDOS_MISC_H
#define PSEUDOS_MISC_H

void halt();

// printf with fflush
void printk(const char* fmt, ...);

void panic0(const char* str);
void panic(const char* fmt, ...);

void assertion_failure(char* exp, char* file, char* base_file, int line);
#define assert(exp) if(exp) ; \
	else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__);

#endif
