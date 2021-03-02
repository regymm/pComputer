/**
 * File              : global.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.02.24
 */

#include "global.h"
#include "process.h"

Process proc_table[PROC_NUM_MAX];
ProcManager procmanager;
unsigned int ticks = 0;

void cli()
{
	csrw_mstatus(0x00000000);

}
void sti()
{
	csrw_mstatus(0x00000088);
}
