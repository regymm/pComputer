int video_x;
int video_y;
int dummy_arr[1000];
volatile int* video_base		= (int*) 0x94000000;
void hdmi_putchar(char c);
int main()
{
	video_x = 0;
	video_y = 0;
	while (1) {
		int i;
		for(i = 0; i < 50000; i++);
		hdmi_putchar('0');
		hdmi_putchar('x');
		/*hdmi_putchar(((int)main >> 28) + '0');*/
		/*hdmi_putchar(((int)main >> 24) + '0');*/
		/*hdmi_putchar(((int)main >> 20) + '0');*/
		/*hdmi_putchar(((int)main >> 16) + '0');*/
		/*hdmi_putchar(((int)main >> 12) + '0');*/
		/*hdmi_putchar(((int)main >> 8) + '0');*/
		/*hdmi_putchar(((int)main >> 4) + '0');*/
		/*hdmi_putchar(((int)main >> 0) + '0');*/
		hdmi_putchar('\n');
	}
	return 0;
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
