#ifndef	_IIC
	#define _IIC
	#include "..\sameShit.h"
	void IIC_Start();
	void IIC_Stop();
	void IIC_SendACK(bit ack);
	bit IIC_RecvACK();
	void IIC_SendByte(uchar dat);
	uchar IIC_RecvByte();
#endif	
