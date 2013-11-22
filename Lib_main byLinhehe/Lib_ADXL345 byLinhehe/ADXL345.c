#include "..\sameShit.h"
#include "ADXL345.h"
#ifndef _IIC_PINS
#define _IIC_PINS
	sbit	  SCL=P1^0;      //IIC时钟引脚定义
	sbit 	  SDA=P1^1;      //IIC数据引脚定义
#endif
uchar BUF[8];                         //接收数据缓存区      	
int  data_xyz[3];					 //读取数值保存于该变量
//******单字节写入*******************************************
void Single_Write_ADXL345(uchar REG_Address,uchar REG_data)
{
    IIC_Start();                  //起始信号
    IIC_SendByte(SlaveAddress);   //发送设备地址+写信号
    IIC_SendByte(REG_Address);    //内部寄存器地址，请参考中文pdf22页 
    IIC_SendByte(REG_data);       //内部寄存器数据，请参考中文pdf22页 
    IIC_Stop();                   //发送停止信号
}

//********单字节读取*****************************************
uchar Single_Read_ADXL345(uchar REG_Address)
{  uchar REG_data;
    IIC_Start();                          //起始信号
    IIC_SendByte(SlaveAddress);           //发送设备地址+写信号
    IIC_SendByte(REG_Address);                   //发送存储单元地址，从0开始	
    IIC_Start();                          //起始信号
    IIC_SendByte(SlaveAddress+1);         //发送设备地址+读信号
    REG_data=IIC_RecvByte();              //读出寄存器数据
	IIC_SendACK(1);   
	IIC_Stop();                           //停止信号
    return REG_data; 
}
//*********************************************************
//连续读出ADXL345内部加速度数据，地址范围0x32~0x37
//*********************************************************
void Multiple_read_ADXL345(void)
{   uchar i;
    IIC_Start();                          //起始信号
    IIC_SendByte(SlaveAddress);           //发送设备地址+写信号
    IIC_SendByte(0x32);                   //发送存储单元地址，从0x32开始	
    IIC_Start();                          //起始信号
    IIC_SendByte(SlaveAddress+1);         //发送设备地址+读信号
	 for (i=0; i<6; i++)                      //连续读取6个地址数据，存储中BUF
    {
        BUF[i] = IIC_RecvByte();          //BUF[0]存储0x32地址中的数据
        if (i == 5)						  //1-0 x 3-2 y 5-4 z
        {
           IIC_SendACK(1);                //最后一个数据需要回NOACK
        }
        else
        {
          IIC_SendACK(0);                //回应ACK
       }
   }
    IIC_Stop();                          //停止信号
    Delay5ms();
	data_xyz[0]=(BUF[1]<<8)+BUF[0];  //合成数据   
	data_xyz[1]=(BUF[3]<<8)+BUF[2];  //合成数据   
	data_xyz[2]=(BUF[5]<<8)+BUF[4];  //合成数据   
}


//初始化ADXL345，根据需要请参考pdf进行修改************************
void Init_ADXL345()
{
   Single_Write_ADXL345(0x31,0x0B);   //测量范围,正负16g，13位模式
   Single_Write_ADXL345(0x2C,0x08);   //速率设定为12.5 参考pdf13页
   Single_Write_ADXL345(0x2D,0x08);   //选择电源模式   参考pdf24页
   Single_Write_ADXL345(0x2E,0x80);   //使能 DATA_READY 中断
   Single_Write_ADXL345(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
   Single_Write_ADXL345(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}
