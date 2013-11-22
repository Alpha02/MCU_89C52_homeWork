#include "IIC.h"
#ifndef _IIC_PINS
#define _IIC_PINS
	sbit	  SCL=P1^0;      //IIC时钟引脚定义
	sbit 	  SDA=P1^1;      //IIC数据引脚定义
#endif
/**************************************
开始信号
**************************************/
void IIC_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}
/**************************************
停止信号
**************************************/
void IIC_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}
/**************************************
发送应答信号
入口参数:ack (0:ACK 1:NAK)
**************************************/
void IIC_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}
/**************************************
接收应答信号
**************************************/
bit IIC_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号(利用了CY标志位)
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}
/**************************************
向IIC总线发送一个字节数据
**************************************/
void IIC_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
   	IIC_RecvACK();
}
/**************************************
从IIC总线接收一个字节数据
**************************************/
uchar IIC_RecvByte()
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    return dat;
}

