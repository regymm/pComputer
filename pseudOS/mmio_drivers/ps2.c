/**
 * File              : ps2.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.05.04
 * Last Modified Date: 2021.05.04
 */

#include "ps2.h"

#define KEYCODE_LENGTH (0x7F)

#define K_TAB 1001
#define K_SHIFT_L 1002
#define K_BS 1003
#define K_ESC 1004

volatile int* ps2_keycode		= (int*) 0x9a000000;

int keycode_map [KEYCODE_LENGTH * 2] = {
	/* 0x00 */	'\0',		'\0',
	/* 0x01 */	'\0',		'\0',
	/* 0x02 */	'\0',		'\0',
	/* 0x03 */	'\0',		'\0',
	/* 0x04 */	'\0',		'\0',
	/* 0x05 */	'\0',		'\0',
	/* 0x06 */	'\0',		'\0',
	/* 0x07 */	'\0',		'\0',
	/* 0x08 */	'\0',		'\0',
	/* 0x09 */	'\0',		'\0',
	/* 0x0a */	'\0',		'\0',
	/* 0x0b */	'\0',		'\0',
	/* 0x0c */	'\0',		'\0',
	/* 0x0d */	K_TAB,		K_TAB,
	/* 0x0e */	'\0',		'\0',
	/* 0x0f */	'\0',		'\0',
	/* 0x10 */	'\0',		'\0',
	/* 0x11 */	'\0',		'\0',
	/* 0x12 */	K_SHIFT_L,	K_SHIFT_L,
	/* 0x13 */	'\0',		'\0',
	/* 0x14 */	'\0',		'\0',
	/* 0x15 */	'\0',		'\0',
	/* 0x16 */	'0',		'!',
	/* 0x17 */	'\0',		'\0',
	/* 0x18 */	'\0',		'\0',
	/* 0x19 */	'\0',		'\0',
	/* 0x1a */	'\0',		'\0',
	/* 0x1b */	'\0',		'\0',
	/* 0x1c */	'a',		'A',
	/* 0x1d */	'\0',		'\0',
	/* 0x1e */	'2',		'@',
	/* 0x1f */	'\0',		'\0',
	/* 0x20 */	'\0',		'\0',
	/* 0x21 */	'c',		'C',
	/* 0x22 */	'\0',		'\0',
	/* 0x23 */	'\0',		'\0',
	/* 0x24 */	'\0',		'\0',
	/* 0x25 */	'4',		'$',
	/* 0x26 */	'3',		'#',
	/* 0x27 */	'\0',		'\0',
	/* 0x28 */	'\0',		'\0',
	/* 0x29 */	'\0',		'\0',
	/* 0x2a */	'\0',		'\0',
	/* 0x2b */	'\0',		'\0',
	/* 0x2c */	'\0',		'\0',
	/* 0x2d */	'\0',		'\0',
	/* 0x2e */	'5',		'%',
	/* 0x2f */	'\0',		'\0',
	/* 0x30 */	'\0',		'\0',
	/* 0x31 */	'\0',		'\0',
	/* 0x32 */	'b',		'B',
	/* 0x33 */	'\0',		'\0',
	/* 0x34 */	'\0',		'\0',
	/* 0x35 */	'\0',		'\0',
	/* 0x36 */	'6',		'^',
	/* 0x37 */	'\0',		'\0',
	/* 0x38 */	'\0',		'\0',
	/* 0x39 */	'\0',		'\0',
	/* 0x3a */	'\0',		'\0',
	/* 0x3b */	'\0',		'\0',
	/* 0x3c */	'\0',		'\0',
	/* 0x3d */	'7',		'&',
	/* 0x3e */	'8',		'*',
	/* 0x3f */	'\0',		'\0',
	/* 0x40 */	'\0',		'\0',
	/* 0x41 */	'\0',		'\0',
	/* 0x42 */	'\0',		'\0',
	/* 0x43 */	'\0',		'\0',
	/* 0x44 */	'\0',		'\0',
	/* 0x45 */	'\0',		'\0',
	/* 0x46 */	'9',		'(',
	/* 0x47 */	'\0',		'\0',
	/* 0x48 */	'\0',		'\0',
	/* 0x49 */	'\0',		'\0',
	/* 0x4a */	'\0',		'\0',
	/* 0x4b */	'\0',		'\0',
	/* 0x4c */	'\0',		'\0',
	/* 0x4d */	'\0',		'\0',
	/* 0x4e */	'-',		'_',
	/* 0x4f */	'\0',		'\0',
	/* 0x50 */	'\0',		'\0',
	/* 0x51 */	'\0',		'\0',
	/* 0x52 */	'\0',		'\0',
	/* 0x53 */	'\0',		'\0',
	/* 0x54 */	'\0',		'\0',
	/* 0x55 */	'=',		'+',
	/* 0x56 */	'\0',		'\0',
	/* 0x57 */	'\0',		'\0',
	/* 0x58 */	'\0',		'\0',
	/* 0x59 */	'\0',		'\0',
	/* 0x5a */	'\0',		'\0',
	/* 0x5b */	'\0',		'\0',
	/* 0x5c */	'\0',		'\0',
	/* 0x5d */	'\0',		'\0',
	/* 0x5e */	'\0',		'\0',
	/* 0x5f */	'\0',		'\0',
	/* 0x60 */	'\0',		'\0',
	/* 0x61 */	'\0',		'\0',
	/* 0x62 */	'\0',		'\0',
	/* 0x63 */	'\0',		'\0',
	/* 0x64 */	'\0',		'\0',
	/* 0x65 */	'\0',		'\0',
	/* 0x66 */	K_BS,		K_BS,
	/* 0x67 */	'\0',		'\0',
	/* 0x68 */	'\0',		'\0',
	/* 0x69 */	'\0',		'\0',
	/* 0x6a */	'\0',		'\0',
	/* 0x6b */	'\0',		'\0',
	/* 0x6c */	'\0',		'\0',
	/* 0x6d */	'\0',		'\0',
	/* 0x6e */	'\0',		'\0',
	/* 0x6f */	'\0',		'\0',
	/* 0x70 */	'\0',		'\0',
	/* 0x71 */	'\0',		'\0',
	/* 0x72 */	'\0',		'\0',
	/* 0x73 */	'\0',		'\0',
	/* 0x74 */	'\0',		'\0',
	/* 0x75 */	'\0',		'\0',
	/* 0x76 */	K_ESC,		K_ESC,
	/* 0x77 */	'\0',		'\0',
	/* 0x78 */	'\0',		'\0',
	/* 0x79 */	'\0',		'\0',
	/* 0x7a */	'\0',		'\0',
	/* 0x7b */	'\0',		'\0',
	/* 0x7c */	'\0',		'\0',
	/* 0x7d */	'\0',		'\0',
	/* 0x7e */	'\0',		'\0',
};

int ps2_decode(unsigned int keycode)
{
	static int last_keycode = 0;
	static int control_on = 0;
	static int shift_on = 0;
}
