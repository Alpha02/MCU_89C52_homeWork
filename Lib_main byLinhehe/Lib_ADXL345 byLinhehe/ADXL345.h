#ifndef _ADXL345
	#define _ADXL345
	#include "..\sameShit.h"
	#include "IIC.h"
	#define	SlaveAddress   0xA6	  //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
	                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A

	void Init_ADXL345(void);             //��ʼ��ADXL345
	void  Single_Write_ADXL345(uchar REG_Address,uchar REG_data);   //����д������
	uchar Single_Read_ADXL345(uchar REG_Address);                   //������ȡ�ڲ��Ĵ�������
	void  Multiple_Read_ADXL345();                                  //�����Ķ�ȡ�ڲ��Ĵ�������
#endif