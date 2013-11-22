
#include "NRF24L01.h" 

sbit 	MISO	=P1^5;
sbit 	MOSI	=P1^1;
sbit	SCK	    =P1^6;
sbit	CE	    =P1^7;
sbit	CSN		=P1^2;
sbit	IRQ		=P1^0;

uint 	bdata sta;   //״̬��־
sbit	Flag_Receive	=sta^6;
sbit	Flag_Send		=sta^5;
sbit	MAX_ReSend		=sta^4;
uint const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
uint const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//�������ݸ��õ�ַ


void NRF_Init(void){
	Delay_us(100);
 	CE=0;    // chip enable
 	CSN=1;   // Spi disable 
 	SCK=0;   // Spi clock line init high
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_WriteReg(WRITE_REG + EN_AA, 0x01);      //  Ƶ��0�Զ�	ACKӦ������	
	SPI_WriteReg(WRITE_REG + EN_RXADDR, 0x01);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_WriteReg(WRITE_REG + RF_CH, 40);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	SPI_WriteReg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ20�ֽ�
	SPI_WriteReg(WRITE_REG + RF_SETUP, 0x07);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB
}
void NRF_setMode_Receive(){
	CE=0;
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1; 
	Delay_us(130);
}
void NRF_setMode_Send(){
	CE=0;
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0e); 			// IRQ�շ�����ж���Ӧ��16λCRC��������
	CE = 1; 
	Delay_us(130);
}
void NRF_Send(uchar * buf_send){
   	CE=0;			//StandBy Iģʽ	
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(WR_TX_PLOAD, buf_send, TX_PLOAD_WIDTH); 			 // װ������	
	SPI_WriteReg(WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	CE=1;		 //�ø�CE���������ݷ���
	Delay_us(10);
}
void NRF_Receive(uchar * buf_recv){
	//uchar revale=0;
	sta=SPI_ReadReg(STATUS);	// ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(Flag_Receive)				// �ж��Ƿ���յ�����
	{
	    CE = 0; 			//SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD,buf_recv,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		//revale =1;			//��ȡ������ɱ�־
	}
	SPI_WriteReg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
}