/**
 * File              : CH375.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.04
 * Last Modified Date: 2021.03.04
 */
#include "../kernel/global.h"
#include "../kernel/misc.h"
#include "CH375_low.h"
#include "CH375.h"
#include "stdio.h"
#include "../include/mmio_basic.h"

volatile int* usb_tx_cmd		= (int*) 0x97000000;
volatile int* usb_tx_data		= (int*) 0x97000008;
volatile int* usb_tx_done		= (int*) 0x97000008;
volatile int* usb_rx_reset		= (int*) 0x97000004;
volatile int* usb_rx_new		= (int*) 0x97000004;
volatile int* usb_rx_data		= (int*) 0x97000000;
volatile int* usb_nint			= (int*) 0x9700000c;

/*uint8_t endp_out_addr;*/
/*uint8_t endp_out_size;*/
/*uint8_t endp_in_addr;*/
/*uint8_t endp6_mode, endp7_mode;*/
/*uint8_t recv_buffer[CH375_MAX_DATA_LEN];*/
/*uint8_t *cmd_buf;*/
/*uint8_t *ret_buf;*/

/*int BlockPerSector;*/

void CH375_WR(unsigned char c)
{
	while(! *usb_tx_done);
	/*printf("a");*/
	*usb_tx_cmd = c;
	while(! *usb_tx_done);
	//printf("WR %x\r\n", c);
}
void CH375_WR_DATA(unsigned char c)
{
	while(! *usb_tx_done);
	*usb_tx_data = c;
	while(! *usb_tx_done);
	//printf("WR %x\r\n", c);
}
unsigned char CH375_RD()
{
	int i = 0;
	while(! *usb_rx_new) i++;
	unsigned char c = *usb_rx_data;
	*usb_rx_reset = 1;
	//printf("CH375 RD after %d tries\r\n", i);
	return c;
}
int CH375_INT()
{
	return ! *usb_nint;
}
int set_usb_mode(int mode)
{
  CH375_WR(CMD_SET_USB_MODE);
  CH375_WR_DATA(mode);
  /*endp6_mode=endp7_mode=0x80;*/
  return CH375_RD();
}
int getIrq()
{
  while(!CH375_INT());
  CH375_WR(CMD_GET_STATUS);
  int i;
  for (i = 0; i < 1000; i++);
  unsigned char c = CH375_RD();
  /*if (c != 0x2c) printf("getIRQ: INT %x\r\n", c);*/
  /*printf("getIRQ: INT %x\r\n", c);*/
  /*fflush(stdout);*/
  return c;
}
/*int getIrq_first()*/
/*{*/
  /*CH375_WR(CMD_GET_STATUS);*/
  /*printf("getIRQ: First IRQ\r\n");*/
  /*return CH375_RD();*/
/*}*/
/*unsigned char get_descr( unsigned char type ) {*/
  /*CH375_WR( CMD_GET_DESCR );*/
  /*CH375_WR_DATA( type );*/
  /*return getIrq();*/
/*}*/
/*uint8_t rd_usb_data( uint8_t *buf )*/
/*{*/
  /*uint8_t i, len;*/
  /*CH375_WR( CMD_RD_USB_DATA );*/
  /*len=CH375_RD();*/
  /*for ( i=0; i!=len; i++ ) *buf++=CH375_RD();*/
  /*return( len );*/
/*}*/
/*uint8_t host_recv()*/
/*{*/
	/*int len = rd_usb_data(recv_buffer);*/
	/*return len;*/
/*}*/
/*uint8_t set_config(uint8_t cfg){*/
  /*[>endp6_mode=endp7_mode=0x80; // reset the sync flags<]*/
  /*CH375_WR(CMD_SET_CONFIG);*/
  /*CH375_WR_DATA(cfg);*/
  /*return getIrq();*/
/*}*/
/*void toggle_recv()*/
/*{*/
  /*CH375_WR( CMD_SET_ENDP6 );*/
  /*CH375_WR_DATA( endp6_mode );*/
  /*endp6_mode^=0x40;*/
/*}*/
/*uint8_t issue_token( uint8_t endp_and_pid )*/
/*{*/
  /*CH375_WR( CMD_ISSUE_TOKEN );*/
  /*CH375_WR_DATA( endp_and_pid );*/
  /*return  getIrq();*/
/*}*/
/*uint8_t set_addr( uint8_t addr )*/
/*{*/
  /*uint8_t irq;*/
  /*CH375_WR(CMD_SET_ADDRESS);*/
  /*CH375_WR_DATA(addr);*/
  /*irq = getIrq();*/
  /*CH375_WR(CMD_SET_USB_ADDR);*/
  /*CH375_WR_DATA(addr);*/
  /*return irq;*/
/*}*/
void setupUSB(){

	/*CH375_WR(CMD_DISK_RESET);*/
	/*for (i = 0; i < 10000; i++);*/
	/*CH375_WR(CMD_DISK_INIT);*/
	/*for (i = 0; i < 10000; i++);*/
	/*while(getIrq() != USB_INT_SUCCESS);*/
	/*printf("USB Disk Ready.\r\n");*/
	/*//printf("First IRQ \r\n");*/
	/*//set_usb_mode( 7 );*/
	/*//for (i = 0; i < 10000; i++);*/

	/*//delay(10);*/
	/*for (i = 0; i < 10000; i++);*/
	/*set_usb_mode( 6 );*/
	/*for (i = 0; i < 10000; i++);*/
	/*////delay(10);*/
	/*while(getIrq()!=USB_INT_CONNECT);*/
	/*printf("Mode done \r\n");*/
	/*for (i = 0; i < 10000; i++);*/
	/*int irq = get_descr(1);*/
	/*printf("get_descr done: %x \r\n", irq);*/
	/*int len = 0;*/
	/*if(irq==USB_INT_SUCCESS){*/
	   /*len = rd_usb_data( recv_buffer );*/
	/*}*/
	/*printf("len determined: %d \r\n", len);*/
	/*irq = set_addr(2);  */
	/*printf("RD %x\r\n", irq);*/
	/*if(irq==USB_INT_SUCCESS){*/
	  /*printf("set_addr 2 success \r\n");*/
	  /*irq = get_descr(2); */
	   /*if(irq==USB_INT_SUCCESS){*/
		   /*printf("get_descr 2 success \r\n");*/
		   /*len = rd_usb_data( recv_buffer );*/
		   /*if(p_cfg_descr->endp_descr[0].bDescriptorType==0x21){ // skip hid des*/
			/*tmpEp = (PUSB_ENDP_DESCR)((int8_t*)(&(p_cfg_descr->endp_descr[0]))+p_cfg_descr->endp_descr[0].bLength); // get the real ep position*/
		  /*}*/
		   /*endp_out_addr=endp_in_addr=0;*/
		  /*int address =tmpEp->bEndpointAddress;*/
		/*if( address&0x80 ){*/
		  /*endp_in_addr = address&0x0f;*/
		/*}else{*/
		  /*endp_out_addr = address&0x0f;*/
		  /*endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;*/
		  /*if( endp_out_size == 0 || endp_out_size > 64 )*/
			/*endp_out_size = 64;*/
		/*}*/
		  /*irq = set_config(p_cfg_descr->cfg_descr.bConfigurationvalue);*/
		/*if(irq==USB_INT_SUCCESS){*/
			/*CH375_WR( CMD_SET_RETRY );*/
			/*CH375_WR( 0x25 );*/
			/*CH375_WR( 0x85 );*/
			/*//isConnected = true; */
			/*toggle_recv();*/
			/*while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);*/
		  /*}*/
	   /*}*/
	/*}*/
}

int USBDiskInit(CH375Disk* udisk)
{
	*usb_rx_reset = 1;

	printk("mmio_devices: USBDiskInit start... \r\n");
	int i;
	unsigned char c;

	CH375_WR(CMD_RESET_ALL);
	for (i = 0; i < 10000; i++);

	c = set_usb_mode(6);
	printk("mmio_devices: set_usb_mode %x\r\n", c);
	/*if ((c = getIrq_first()) != USB_INT_CONNECT) {*/
		/*printf("RD %x\r\n", c);*/
		/*fflush(stdout);*/
		while((c = getIrq()) != USB_INT_CONNECT) {
			/*printf("RD %x\r\n", c);*/
			/*fflush(stdout);*/
		}
	/*}*/

	for (i = 0; i < 10000; i++);
	CH375_WR(CMD_DISK_INIT);
	for (i = 0; i < 10000; i++);
	printk("mmio_devices: Waiting for ready...\r\n");
	while((c = getIrq()) != USB_INT_SUCCESS) {
		printk("mmio_devices: irq: %x\r\n", c);
	}
	printk("mmio_devices: USB disk init done.\r\n");

	CH375_WR(CMD_DISK_SIZE);
	if (getIrq() != USB_INT_SUCCESS)
		printk("USB disk size err!\r\n");
	CH375_WR(CMD_RD_USB_DATA);
	i = CH375_RD();
	int data_buffer[4096];
	if (i != 8) printk("RD USB DATA not 8!\r\n");
	for (i = 0; i != 8; i++) {
		data_buffer[i] = CH375_RD();
		/*printk("%x\r\n", data_buffer[i]);*/
	}
	unsigned int usb_sector_size = (data_buffer[6] << 8) + data_buffer[7];
	printk("USB sector size: 0x%x\r\n", usb_sector_size);
	unsigned int usb_sector_number = (data_buffer[0] << 24) + (data_buffer[1] << 16) + (data_buffer[2] << 8) + (data_buffer[3] << 0);
	unsigned long usb_size_bytes = usb_sector_number * usb_sector_size / 2;
	printk("USB disk size: %d sectors, %lld bytes\r\n", usb_sector_number, usb_size_bytes);

	udisk->BlockPerSector = usb_sector_size / CH375_BLOCK_SIZE;

	CH375_WR(CMD_SET_PKT_P_SEC);
	CH375_WR_DATA(0x39);
	CH375_WR_DATA(udisk->BlockPerSector);
	return 0;
}
int USBReadOneSector(CH375Disk* udisk, unsigned int sector_num)
{
	udisk->CurrentSector = sector_num;
	CH375_WR(CMD_DISK_READ);
	CH375_WR_DATA((unsigned char)sector_num);
	CH375_WR_DATA((unsigned char)(sector_num >> 8));
	CH375_WR_DATA((unsigned char)(sector_num >> 16));
	CH375_WR_DATA((unsigned char)(sector_num >> 24));
	CH375_WR_DATA((unsigned char)1); // read only one sector to buf
	int i;
	int iblk;
	int irq;
	unsigned char c;
	for (iblk = 0; iblk < udisk->BlockPerSector; iblk++) {
		int irq = getIrq();
		if (irq == USB_INT_DISK_READ) {
			CH375_WR(CMD_RD_USB_DATA);
			int len = CH375_RD();
			/*printf("len: %d\r\n", len);*/
			for (i = 0; i < len; i++) {
				c = CH375_RD();
				printk("%02x", c);
				udisk->buf[i] = c;
			}
			printk("\r\n");
			CH375_WR(CMD_DISK_RD_GO);
		}
		else {
			printk("USBReadOneSector error\r\n");
			break;
		}
	}
	if (iblk == udisk->BlockPerSector) {
		irq = getIrq();
		if (irq == USB_INT_SUCCESS) return 0;
	}
	USBClearError();
	return irq;
}

int USBWriteBackBuf(CH375Disk* udisk)
{
	CH375_WR(CMD_DISK_WRITE);
	CH375_WR_DATA((unsigned char) udisk->CurrentSector);
	CH375_WR_DATA((unsigned char) (udisk->CurrentSector >> 8));
	CH375_WR_DATA((unsigned char) (udisk->CurrentSector >> 16));
	CH375_WR_DATA((unsigned char) (udisk->CurrentSector >> 24));
	CH375_WR_DATA(1);
	int i;
	int iblk;
	int irq;
	int ibuf = 0;
	for (iblk = 0; iblk < udisk->BlockPerSector; iblk++) {
		irq = getIrq();
		if (irq == USB_INT_DISK_WRITE) {
			CH375_WR(CMD_WR_USB_DATA7);
			CH375_WR(CH375_BLOCK_SIZE);
			for (i = 0; i < CH375_BLOCK_SIZE; i++) {
				CH375_WR_DATA(udisk->buf[ibuf++]);
			}
			CH375_WR(CMD_DISK_WR_GO);
		}
		else {
			printk("USBWriteBackBuf error\r\n");
			break;
		}
	}
	if (iblk == udisk->BlockPerSector) {
		irq = getIrq();
		if (irq == USB_INT_SUCCESS) return 0;
	}
	USBClearError();
	return irq;
}

void USBClearError()
{
	printk("USBClearError: error occured before!\r\n");
	CH375_WR(CMD_DISK_R_SENSE);
	getIrq();
}

void usb_test()
{
	/**usb_rx_reset = 1;*/

	CH375Disk udisk;
	USBDiskInit(&udisk);
	USBReadOneSector(&udisk, 0);
	int i;
	for(i = 0; i < 0x200; i++)
		udisk.buf[i] = (unsigned char)i;
	USBWriteBackBuf(&udisk);

	/*setupUSBDisk();*/
	/*setupUSB();*/
	/*printf("setupUSB done. \r\n");*/

	/*while(1){*/
		/*int len = host_recv();*/
		/*if(len>0){*/
			/*for(int i=0;i<len;i++){*/
				/*printf("%x, ", recv_buffer[i]);*/
			/*}*/
			/*printf("\r\n");*/
		/*}else{*/
		/*}*/
		/*toggle_recv();   */
		/*while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);*/
	/*}*/

}
