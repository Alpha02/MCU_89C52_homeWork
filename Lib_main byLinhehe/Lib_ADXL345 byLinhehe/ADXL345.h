#ifndef _ADXL345
	#define _ADXL345
	#include "..\sameShit.h"
	#include "IIC.h"
	#define	SlaveAddress   0xA6	  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
	                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A

	void Init_ADXL345(void);             //初始化ADXL345
	void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //单个写入数据
	uchar Single_Read_ADXL345(uchar REG_Address);                   //单个读取内部寄存器数据
	void  Multiple_Read_ADXL345();                                  //连续的读取内部寄存器数据
#endif