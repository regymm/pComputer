/**
 * File              : blk.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.12.23
 * Last Modified Date: 2021.12.23
 */
#include "blk.h"
#include "stdio.h"

int blk_seek_and_read(Blk* blk, unsigned long seek, unsigned long bytes, char* buf)
{
	/*printf("%x %d\n", seek, bytes);*/
	if (!blk->obtained) return -1;
	unsigned int sector_start = seek / 0x200;
	unsigned int sector_end = (seek + bytes) / 0x200;
	unsigned int bytes_start = seek % 0x200;
	unsigned int bytes_end = (seek + bytes) % 0x200;
	/*if (bytes_end == 0) bytes_end = 0x200;*/
	unsigned int sec, byte;
	unsigned int cnt = 0;
	if (blk->isdirty(blk)) blk->writeback(blk);
	for (sec = sector_start; sec <= sector_end; sec++) {
		blk->ioctl(blk, 0, &sec);
		blk->load(blk);
		unsigned int curr_bytes_start = sec == sector_start ? bytes_start : 0;
		unsigned int curr_bytes_end   = sec == sector_end   ? bytes_end : 0x200;
		/*printf("%d %d\n\n", curr_bytes_start, curr_bytes_end);*/
		for (byte = curr_bytes_start; byte < curr_bytes_end; byte++, cnt++) {
			// TODO: only work on little endian
			// this problem lies deep inside my Blk structure
			/*buf[cnt] = (blk->cache[byte / 4] >> (byte % 4)) % 256;*/
			buf[cnt] = blk->cache[byte / 4] >> ((byte % 4) * 8);
			/*printf("%08x ", blk->cache[byte/4]);*/
			/*printf("%02x", buf[cnt]);*/
		}
	}
	/*printf("%d\n", cnt);*/
	return cnt;
}
