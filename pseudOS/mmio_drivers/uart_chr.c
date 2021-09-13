#include "basic.h"
#include "uart_chr.h"

extern Uart uart0;
extern Chr uartchr0;

int get_uart_chr(Chr* uartchr, Uart* u)
{
	uartchr->obtained = 0;

	uartchr->obtain = uart_chr_obtain;
	uartchr->release = uart_chr_release;
	uartchr->ioctl = uart_chr_ioctl;
	uartchr->read = uart_chr_read;
	uartchr->write = uart_chr_write;
	uartchr->isready = uart_chr_isready;

	uartchr->obj = (void*) u;
	return 0;
}

int uart_chr_obtain(Chr* self)
{
	self->obtained = 1;
	return 0;
}

int uart_chr_release(Chr* self)
{
	self->obtained = 0;
	return 0;
}

int uart_chr_ioctl(Chr* self, int op, void* param)
{
	return -1;
}

int uart_chr_write(Chr* self, int size, char* str)
{
	Uart* u = self->obj;
	int i;
	while(! *u->tx_done);
	for (i = 0; i < size; i++) {
		*u->tx = str[i];
		while(! u->tx_done);
	}
	return size;
}

int uart_chr_read(Chr *self, int size, char *str)
{
	Uart* u = self->obj;
	int i;
	// TODO: how many u->rx_reset required
	for (i = 0; i < size; i++) {
		while(! *u->rx_new);
		str[i] = *u->rx_data;
		*u->rx_reset = 1;
	}
	return size;
}

// didn't specify TX/RX so no much use
int uart_chr_isready(Chr* self)
{
	/*Uart* u = self->obj;*/
	// 1 means ready
	return 1;
}
