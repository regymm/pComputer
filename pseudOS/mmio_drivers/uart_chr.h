/**
 * File              : uart_chr.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.05
 * Last Modified Date: 2021.09.05
 */
#ifndef PSEUDOS_UART_CHR_H
#define PSEUDOS_UART_CHR_H

#include "uart.h"
#include "../kernel/chr.h"

int get_uart_chr(Chr* uartchr, Uart* u);

int uart_chr_obtain(Chr* self);
int uart_chr_release(Chr* self);
int uart_chr_ioctl(Chr* self, int op, void* param);
int uart_chr_write(Chr* self, int size, char* str);
int uart_chr_read(Chr* self, int size, char* str);
int uart_chr_isready(Chr* self);

#endif
