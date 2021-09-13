#include "basic.h"
#include "uart.h"

int get_uart_0(Uart* u)
{
	u->tx =			uart_tx;
	u->tx_done =	uart_tx_done;
	u->rx_reset=	uart_rx_reset;
	u->rx_new=		uart_rx_new;
	u->rx_data =	uart_rx_data;
	return 0;
}
