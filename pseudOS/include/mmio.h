// pComputer MMIO library
#ifndef MMIO_H
#define MMIO_H
#include "mmio_basic.h"
#include "CH375.h"
#include "stdio.h"

volatile int* usb_tx_cmd		= (int*) 0x97000000;
volatile int* usb_tx_data		= (int*) 0x97000008;
volatile int* usb_tx_done		= (int*) 0x97000008;
volatile int* usb_rx_reset		= (int*) 0x97000004;
volatile int* usb_rx_new		= (int*) 0x97000004;
volatile int* usb_rx_data		= (int*) 0x97000000;
volatile int* usb_nint			= (int*) 0x9700000c;

void CH375_WR(unsigned char c);
unsigned char CH375_RD();

void CH375_WR(unsigned char c)
{
	while(! *usb_tx_done);
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
  endp6_mode=endp7_mode=0x80;
  return CH375_RD();
}
int getIrq()
{
  while(!CH375_INT());
  CH375_WR(CMD_GET_STATUS);
  int i;
  for (i = 0; i < 1000; i++);
  unsigned char c = CH375_RD();
  if (c != 0x2c) printf("getIRQ: INT %x\r\n", c);
  return c;
}
int getIrq_first()
{
  CH375_WR(CMD_GET_STATUS);
  uart_putstr("getIRQ: First IRQ\r\n");
  return CH375_RD();
}
unsigned char get_descr( unsigned char type ) {
  CH375_WR( CMD_GET_DESCR );
  CH375_WR_DATA( type );
  return getIrq();
}
uint8_t rd_usb_data( uint8_t *buf )
{
  uint8_t i, len;
  CH375_WR( CMD_RD_USB_DATA );
  len=CH375_RD();
  for ( i=0; i!=len; i++ ) *buf++=CH375_RD();
  return( len );
}
uint8_t host_recv()
{
    int len = rd_usb_data(recv_buffer);
    return len;
}
uint8_t set_config(uint8_t cfg){
  endp6_mode=endp7_mode=0x80; // reset the sync flags
  CH375_WR(CMD_SET_CONFIG);
  CH375_WR_DATA(cfg);
  return getIrq();
}
void toggle_recv()
{
  CH375_WR( CMD_SET_ENDP6 );
  CH375_WR_DATA( endp6_mode );
  endp6_mode^=0x40;
}
uint8_t issue_token( uint8_t endp_and_pid )
{
  CH375_WR( CMD_ISSUE_TOKEN );
  CH375_WR_DATA( endp_and_pid );
  return  getIrq();
}
uint8_t set_addr( uint8_t addr )
{
  uint8_t irq;
  CH375_WR(CMD_SET_ADDRESS);
  CH375_WR_DATA(addr);
  irq = getIrq();
  CH375_WR(CMD_SET_USB_ADDR);
  CH375_WR_DATA(addr);
  return irq;
}
void setupUSB(){
	uart_putstr("setupUSB start... \r\n");
	unsigned char c;
	int i;
	c = set_usb_mode(6);
	printf("RD %x\r\n", c);
	printf("First response \r\n");
	if ((getIrq_first()) != USB_INT_CONNECT) {
		//printf("RD %x\r\n", c);
		while(getIrq() != USB_INT_CONNECT);
	}

	for (i = 0; i < 10000; i++);
	CH375_WR(CMD_DISK_INIT);
	for (i = 0; i < 10000; i++);
	for (i = 0; i < 10000; i++);
	for (i = 0; i < 10000; i++);
	for (i = 0; i < 10000; i++);
	while(getIrq() != USB_INT_SUCCESS);
	CH375_WR(CMD_DISK_RESET);
	for (i = 0; i < 10000; i++);
	CH375_WR(CMD_DISK_INIT);
	for (i = 0; i < 10000; i++);
	while(getIrq() != USB_INT_SUCCESS);
	printf("USB Disk Ready.\r\n");
	//uart_putstr("First IRQ \r\n");
	//set_usb_mode( 7 );
	//for (i = 0; i < 10000; i++);

	////delay(10);
	//set_usb_mode( 6 );
	//for (i = 0; i < 10000; i++);
	////delay(10);
	//while(getIrq()!=USB_INT_CONNECT);
	//uart_putstr("Mode done \r\n");
	int irq = get_descr(1);
	printf("get_descr done: %x \r\n", irq);
	int len = 0;
	if(irq==USB_INT_SUCCESS){
	   len = rd_usb_data( recv_buffer );
	}
	printf("len determined: %d \r\n", len);
	irq = set_addr(2);  
	printf("RD %x\r\n", irq);
	if(irq==USB_INT_SUCCESS){
	  uart_putstr("set_addr 2 success \r\n");
	  irq = get_descr(2); 
	   if(irq==USB_INT_SUCCESS){
		   uart_putstr("get_descr 2 success \r\n");
		   len = rd_usb_data( recv_buffer );
		   if(p_cfg_descr->endp_descr[0].bDescriptorType==0x21){ // skip hid des
			tmpEp = (PUSB_ENDP_DESCR)((int8_t*)(&(p_cfg_descr->endp_descr[0]))+p_cfg_descr->endp_descr[0].bLength); // get the real ep position
		  }
		   endp_out_addr=endp_in_addr=0;
		  int address =tmpEp->bEndpointAddress;
		if( address&0x80 ){
		  endp_in_addr = address&0x0f;
		}else{
		  endp_out_addr = address&0x0f;
		  endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;
		  if( endp_out_size == 0 || endp_out_size > 64 )
			endp_out_size = 64;
		}
		  irq = set_config(p_cfg_descr->cfg_descr.bConfigurationvalue);
		if(irq==USB_INT_SUCCESS){
			CH375_WR( CMD_SET_RETRY );
			CH375_WR( 0x25 );
			CH375_WR( 0x85 );
			//isConnected = true; 
			toggle_recv();
			while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);
		  }
	   }
	}
}

#endif
