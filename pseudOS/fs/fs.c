/**
 * File              : fs.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.17
 * Last Modified Date: 2021.02.17
 */
#include "fs.h"

int sdcard_wait_for_ready(SDCard* sd) {
	int i = 1000000;
	while(i--) if(*(sd->mm_ready)) return 0;
	return 1;
}

int sdcard_cleanup(SDCard* sd) {
	return 0;
}
