#include <SPI.h>
/****************************************************************************************************
/SPI单次时序数据交换
/****************************************************************************************************/
	sbit 	MISO	=P1^5;
	sbit 	MOSI	=P1^1;
	sbit	SCK	    =P1^6;
	sbit	CE	    =P1^7;
	sbit	CSN		=P1^2;
	sbit	IRQ		=P1^0;
uchar SPI_swapByte(uchar ch)
{
	uchar i;
   	for(i=0;i<8;i++) // output 8-bit
   	{
		MOSI = (ch & 0x80);           //输出ch当前位的数据至MOSI
		ch = (ch << 1);      	      //ch左移。
		SCK = 1;                 
		ch |= MISO;       		      //接收MISO的数据
		SCK = 0;            	
   	}
    return(ch);
}
/****************************************************************************************************
/SPI读取指定寄存器的值
/****************************************************************************************************/
uchar SPI_Read(uchar reg)
{
	uchar val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	SPI_swapByte(reg);          //写寄存器的地址
	val = SPI_swapByte(0);    //读取寄存器
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(val);        // return register value
}
/****************************************************************************************************/
/*功能：SPI写指定寄存器函数
/****************************************************************************************************/
uchar SPI_Write(uchar reg, uchar val)
{
	uchar status;
	
	CSN = 0;                   // CSN low, init SPI transaction
	status = SPI_swapByte(reg);      // select register
	SPI_swapByte(val);             // ..and write value to it..
	CSN = 1;                   // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}
/****************************************************************************************************/
/*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
/****************************************************************************************************/
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Buf_len)
{
	uchar status,i;
	
	CSN = 0;                    		// Set CSN low, init SPI tranaction
	status = SPI_swapByte(reg);       		// Select register to write to and read status uchar
	
	for(i=0;i<Buf_len;i++)
		pBuf[i] = SPI_swapByte(0);	
	CSN = 1;                           	
	return(status);                    // return nRF24L01 status uchar
}
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Buf_len)
{
	uint status,i;
	
	CSN = 0;            //SPI使能       
	status = SPI_swapByte(reg);   
	for(i=0; i<Buf_len; i++)
		SPI_swapByte(*pBuf++);
	CSN = 1;           //关闭SPI
	return(status);
}