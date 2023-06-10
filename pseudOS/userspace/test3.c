#include "stdio.h"
int main()
{
	while (1) {
		int in;
		scanf("%d", &in);
		printf("input is %d\r\n", in);
		fflush(stdout);
	}
	return 0;
}
