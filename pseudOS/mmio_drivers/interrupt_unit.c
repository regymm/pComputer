/**
 * File              : interrupt_unit.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.05.01
 * Last Modified Date: 2021.05.01
 */

#include "interrupt_unit.h"
volatile int* interrupt_ctrl = (int*) 0x98000000;
