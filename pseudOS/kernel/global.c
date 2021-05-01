/**
 * File              : global.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.02.24
 */

#include "global.h"
#include "process.h"

ProcManager procmanager;
volatile unsigned int ticks = 0;
