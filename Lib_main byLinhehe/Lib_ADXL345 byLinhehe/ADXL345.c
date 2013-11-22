#include "..\sameShit.h"
#include "ADXL345.h"
#ifndef _IIC_PINS
#define _IIC_PINS
	sbit	  SCL=P1^0;      //IICʱ�����Ŷ���
	sbit 	  SDA=P1^1;      //IIC�������Ŷ���
#endif
uchar BUF[8];                         //�������ݻ�����      	
int  data_xyz[3];					 //��ȡ��ֵ�����ڸñ���
//******���ֽ�д��*******************************************
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    IIC_Start();                  //��ʼ�ź�
    IIC_SendByte(SlaveAddress);   //�����豸��ַ+д�ź�
    IIC_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    IIC_SendByte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    IIC_Stop();                   //����ֹͣ�ź�
}

//********���ֽڶ�ȡ*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  uchar REG_data;
    IIC_Start();                          //��ʼ�ź�
    IIC_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    IIC_SendByte(REG_Address);                   //���ʹ洢��Ԫ��ַ����0��ʼ	
    IIC_Start();                          //��ʼ�ź�
    IIC_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
    REG_data=IIC_RecvByte();              //�����Ĵ�������
	IIC_SendACK(1);   
	IIC_Stop();                           //ֹͣ�ź�
    return REG_data; 
}
//*********************************************************
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//*********************************************************
void Multiple_read_ADXL345(void)
{   uchar i;
    IIC_Start();                          //��ʼ�ź�
    IIC_SendByte(SlaveAddress);           //�����豸��ַ+д�ź�
    IIC_SendByte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    IIC_Start();                          //��ʼ�ź�
    IIC_SendByte(SlaveAddress+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = IIC_RecvByte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)						  //1-0 x 3-2 y 5-4 z
        {
           IIC_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          IIC_SendACK(0);                //��ӦACK
       }
   }
    IIC_Stop();                          //ֹͣ�ź�
    Delay5ms();
	data_xyz[0]=(BUF[1]<<8)+BUF[0];  //�ϳ�����   
	data_xyz[1]=(BUF[3]<<8)+BUF[2];  //�ϳ�����   
	data_xyz[2]=(BUF[5]<<8)+BUF[4];  //�ϳ�����   
}


//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x08);   //�����趨Ϊ12.5 �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x05);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}
