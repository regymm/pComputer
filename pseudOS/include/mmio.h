// pComputer MMIO library
#ifndef MMIO_H
#define MMIO_H
#include "mmio_basic.h"

#include "CH375.h"

volatile int* usb_tx			= (int*) 0x97000000;
volatile int* usb_tx_done		= (int*) 0x97000008;
volatile int* usb_rx_begin		= (int*) 0x97000004;
volatile int* usb_rx_done		= (int*) 0x97000004;
volatile int* usb_rx_data		= (int*) 0x97000000;
volatile int* usb_nint			= (int*) 0x9700000c;

void CH375_WR(char c);
char CH375_RD();

void CH375_WR(char c)
{
	while(! *usb_tx_done);
	*usb_tx = c;
	while(! *usb_tx_done);
}
char CH375_RD()
{
	while(! *usb_rx_done);
	*usb_rx_begin = 1;
	while(! *usb_rx_done);
	return *usb_rx_data;
}
int CH375_INT()
{
	return ! *usb_nint;
}
int set_usb_mode(int mode)
{
  CH375_WR(CMD_SET_USB_MODE);
  CH375_WR(mode);
  endp6_mode=endp7_mode=0x80;
  return CH375_RD();
}
int getIrq()
{
  while(!CH375_INT());
  uart_putstr("INT\n\r");
  CH375_WR(CMD_GET_STATUS);
  return CH375_RD();
}
int getIrq_first()
{
  CH375_WR(CMD_GET_STATUS);
  uart_putstr("First IRQ\n\r");
  return CH375_RD();
}
unsigned char get_descr( unsigned char type ) {
  CH375_WR( CMD_GET_DESCR );
  CH375_WR( type );
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
  CH375_WR(cfg);
  return getIrq();
}
void toggle_recv()
{
  CH375_WR( CMD_SET_ENDP6 );
  CH375_WR( endp6_mode );
  endp6_mode^=0x40;
}
uint8_t issue_token( uint8_t endp_and_pid )
{
  CH375_WR( CMD_ISSUE_TOKEN );
  CH375_WR( endp_and_pid );
  return  getIrq();
}
uint8_t set_addr( uint8_t addr )
{
  uint8_t irq;
  CH375_WR(CMD_SET_ADDRESS);
  CH375_WR(addr);
  irq = getIrq();
  CH375_WR(CMD_SET_USB_ADDR);
  CH375_WR(addr);
  return irq;
}
void setupUSB(){
	uart_putstr("setupUSB start... \n\r");
    set_usb_mode( 6 );
	uart_putstr("First response \n\r");
	if (getIrq_first() != USB_INT_CONNECT)
		while(getIrq()!=USB_INT_CONNECT);
	uart_putstr("First IRQ \n\r");
    set_usb_mode( 7 );
    //delay(10);
    set_usb_mode( 6 );
    //delay(10);
    while(getIrq()!=USB_INT_CONNECT);
	uart_putstr("Mode done \n\r");
    int irq = get_descr(1);
	uart_putstr("get_descr done \n\r");
    int len = 0;
    if(irq==USB_INT_SUCCESS){
       len = rd_usb_data( recv_buffer );
    }
	uart_putstr("len determined \n\r");
    irq = set_addr(2);  
    if(irq==USB_INT_SUCCESS){
      irq = get_descr(2); 
       if(irq==USB_INT_SUCCESS){
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
