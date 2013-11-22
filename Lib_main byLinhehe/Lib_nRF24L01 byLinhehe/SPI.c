#include <SPI.h>
/****************************************************************************************************
/SPI����ʱ�����ݽ���
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
		MOSI = (ch & 0x80);           //���ch��ǰλ��������MOSI
		ch = (ch << 1);      	      //ch���ơ�
		SCK = 1;                 
		ch |= MISO;       		      //����MISO������
		SCK = 0;            	
   	}
    return(ch);
}
/****************************************************************************************************
/SPI��ȡָ���Ĵ�����ֵ
/****************************************************************************************************/
uchar SPI_Read(uchar reg)
{
	uchar val;
	
	CSN = 0;                // CSN low, initialize SPI communication...
	SPI_swapByte(reg);          //д�Ĵ����ĵ�ַ
	val = SPI_swapByte(0);    //��ȡ�Ĵ���
	CSN = 1;                // CSN high, terminate SPI communication
	
	return(val);        // return register value
}
/****************************************************************************************************/
/*���ܣ�SPIдָ���Ĵ�������
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
/*����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
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
	
	CSN = 0;            //SPIʹ��       
	status = SPI_swapByte(reg);   
	for(i=0; i<Buf_len; i++)
		SPI_swapByte(*pBuf++);
	CSN = 1;           //�ر�SPI
	return(status);
}