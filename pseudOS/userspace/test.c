#include "stdio.h"
/*#include "math.h"*/
int video_x;
int video_y;
int dummy_arr[1000];
volatile int* video_base		= (int*) 0x94000000;
volatile int* uart_tx			= (int*) 0x93000000;
volatile int* uart_tx_done		= (int*) 0x93000008;
volatile int* uart_rx_reset		= (int*) 0x93000004;
volatile int* uart_rx_new		= (int*) 0x93000004;
volatile int* uart_rx_data		= (int*) 0x93000000;
void uart_putchar(char c);
void uart_putstr(const char* str);
void hdmi_putchar(char c);
int chara = '#';
int main()
{
	video_x = 0;
	video_y = 0;
	uart_putchar(chara);
	while (1) {
		int i, j = 0;
		j++;
		for(i = 0; i < 50000; i++);
		uart_putstr("user_proc\r\n");
		printf("printf test %08x", &i);
		fflush(stdout);
		int a = 10;
		/*int b = sqrt(a);*/
		/*printf("%d", b);*/
		/*return 0;*/
		/*FILE* f = fopen("no.txt", "r");*/
		/*fclose(f);*/
		/*hdmi_putchar('0');*/
		/*hdmi_putchar('x');*/
		/*hdmi_putchar(((int)main >> 28) + '0');*/
		/*hdmi_putchar(((int)main >> 24) + '0');*/
		/*hdmi_putchar(((int)main >> 20) + '0');*/
		/*hdmi_putchar(((int)main >> 16) + '0');*/
		/*hdmi_putchar(((int)main >> 12) + '0');*/
		/*hdmi_putchar(((int)main >> 8) + '0');*/
		/*hdmi_putchar(((int)main >> 4) + '0');*/
		/*hdmi_putchar(((int)main >> 0) + '0');*/
		/*hdmi_putchar('\n');*/
	}
	return 0;
}
void uart_putchar(char c)
{
	while(! *uart_tx_done);
	*uart_tx = c;
	while(! *uart_tx_done);
}
void uart_putstr(const char* str)
{
	int n = 0;
	while(str[n]) uart_putchar(str[n++]);
}
void hdmi_putchar(char c)
{
	if (c == '\r') video_x = 0;
	else if (c == '\n') {
		if (video_y == 29) video_y = 0;
		else video_y++;
	}
	else {
		video_base[video_y * 80 + video_x] = c + 0x0100;
		if (video_x == 79)  {
			video_x = 0;
			if (video_y == 29) video_y = 0;
			else video_y++;
		}
		else video_x++;
	}
}
