#ifndef _SPI
	#define _SPI
	#include "..\sameShit.h"
	uchar SPI_SingleChar(uchar ch);
	uchar SPI_ReadReg(uchar reg);
	uint SPI_WriteReg(uchar reg, uchar value);
	uint SPI_Read_Buf(uchar reg, uchar *pBuf, uchar Buf_len);
	uint SPI_Write_Buf(uchar reg, uchar *pBuf, uchar Buf_len);
#endif