/**
 * File              : misc.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.06
 * Last Modified Date: 2021.03.06
 */
#include "stdio.h"
#include "stdarg.h"
#include "global.h"
#include "../mmio_drivers/basic.h"
#include "isr.h"

void halt()
{
	while (1);
}

void printk(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
}

void panic0(const char* str)
{
	cli();
	uart_putstr(str);
	uart_putstr("PANIC!\r\n");
	halt();

}

void panic(const char* fmt, ...)
{
	cli();
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("PANIC\r\n");
	fflush(stdout);
	halt();
}

void assertion_failure(char* exp, char* file, char* base_file, int line)
{
	cli();
	panic("Assertion failure: %s, %s, %s, %d\r\n", exp, file, base_file, line);
}
