
#include "NRF24L01.h" 

sbit 	MISO	=P1^5;
sbit 	MOSI	=P1^1;
sbit	SCK	    =P1^6;
sbit	CE	    =P1^7;
sbit	CSN		=P1^2;
sbit	IRQ		=P1^0;

uint 	bdata sta;   //状态标志
sbit	Flag_Receive	=sta^6;
sbit	Flag_Send		=sta^5;
sbit	MAX_ReSend		=sta^4;
uint const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
uint const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//发送数据给该地址


void NRF_Init(void){
	Delay_us(100);
 	CE=0;    // chip enable
 	CSN=1;   // Spi disable 
 	SCK=0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_WriteReg(WRITE_REG + EN_AA, 0x01);      //  频道0自动	ACK应答允许	
	SPI_WriteReg(WRITE_REG + EN_RXADDR, 0x01);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_WriteReg(WRITE_REG + RF_CH, 40);        //   设置信道工作为2.4GHZ，收发必须一致
	SPI_WriteReg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为20字节
	SPI_WriteReg(WRITE_REG + RF_SETUP, 0x07);   		//设置发射速率为1MHZ，发射功率为最大值0dB
}
void NRF_setMode_Receive(){
	CE=0;
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0f);   		// IRQ收发完成中断响应，16位CRC	，主接收
	CE = 1; 
	Delay_us(130);
}
void NRF_setMode_Send(){
	CE=0;
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0e); 			// IRQ收发完成中断响应，16位CRC，主发送
	CE = 1; 
	Delay_us(130);
}
void NRF_Send(uchar * buf_send){
   	CE=0;			//StandBy I模式	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	SPI_Write_Buf(WR_TX_PLOAD, buf_send, TX_PLOAD_WIDTH); 			 // 装载数据	
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	CE=1;		 //置高CE，激发数据发送
	Delay_us(10);
}
void NRF_Receive(uchar * buf_recv){
	//uchar revale=0;
	sta=SPI_ReadReg(STATUS);	// 读取状态寄存其来判断数据接收状况
	if(Flag_Receive)				// 判断是否接收到数据
	{
	    CE = 0; 			//SPI使能
		SPI_Read_Buf(RD_RX_PLOAD,buf_recv,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		//revale =1;			//读取数据完成标志
	}
	SPI_WriteReg(WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
}