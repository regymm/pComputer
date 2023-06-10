#include "sys/uio.h"
#include "sys/syscall.h"
#include "stdio.h"

char str[600000000];
int main()
{
	int i;
	for (i = 0; i < 60000; i++) str[i] = "I";
	int cnt = syscall(SYS_write, stdout, str, 50000);
	fflush(stdout);
	printf("%d\n", cnt);
	return 0;
	/*printf("%d", cnt);*/
}
