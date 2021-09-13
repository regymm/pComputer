#ifndef PSEUDOS_UART_H
#define PSEUDOS_UART_H

typedef struct {
	volatile int* tx;
	volatile int* tx_done;
	volatile int* rx_reset;
	volatile int* rx_new;
	volatile int* rx_data;
	short status;
}Uart;

int get_uart_0(Uart* u);

#endif
