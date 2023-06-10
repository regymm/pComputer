/*#include "stdio.h"*/
extern global_d;
int test2(int a, int b)
{
	static int static_c;
	static_c++;
	global_d++;
	/*printf("%d\n", global_d);*/
	return a + b + static_c + global_d;
}
