#include "../sameShit.h"
/**********************************************************************************************************************************************************************/
//程序说明：本程序为12864（st7920）驱动程序，只实现了最简单的显示功能
//端口设置：RS、RW、EN分别为P0口的0、1、2，数据口为P2口用取模软件取的图形或汉字必须是逐行取的，因为本函数是在液晶上逐行打点的
/*******************************************************************************************************************************************************/
#include<stdlib.h>	//内含rand（）函数

//**********宏定义所需指令
#define BASIC_SET  0x30
#define EXTEND_SET 0x34
#define DRAW_ON	   0x36
#define DRAW_OFF   0x34

//*************端口定义
sbit LCD_RS = P0^0;
sbit LCD_RW = P0^1;
sbit LCD_EN = P0^2;

//************变量定义
//uchar dis1[10];


//****************短延时
void delay(uint k)
{
	uint i;
	uchar j;
	for(i = 0; i < k ;i ++)
		for(j = 0; j < 10 ;j ++);
}


//***********ms级延时函数
/*void delay_1ms(uint x)
{
	uint i,j;
	for(j = 0;j < x; j++)
		for(i = 0;i < 110; i++);
} */


//***********12864写指令函数
void write_com(uchar cmd)
{
	LCD_RS = 0;
	LCD_RW = 0;
	P2 = cmd;
	delay(5);
	LCD_EN = 1;
	delay(5);
	LCD_EN = 0;
}


//********12864写数据函数
void write_dat(uchar dat)
{
	LCD_RS = 1;		 
	LCD_RW = 0;
	P2 = dat;
	delay(5);
	LCD_EN = 1;
	delay(5);
	LCD_EN = 0;
}


//****************从LCD中读数据
uchar read_dat(void)
{
	uchar temp;
	P2 = 0XFF;	//释放数据线
	LCD_RS = 1;	   //数据
	LCD_RW = 1;	  // 读模式
	LCD_EN = 1;    //E为高电平进行读数据或指令
	delay(1);
	temp = P2;
	LCD_EN = 0;
	return temp; 
}


//********************************************************
//设置光标（地址）函数
//参数说明：x---为行号，y为列号
//********************************************************
void set_cursor(unsigned char x, unsigned char y)
{
	unsigned char i;
	switch(x)   								//确定行号
	{
		case 0x00: i=0x80; break;				//第一行
		case 0x01: i=0x90; break; 				//第二行
		case 0x02: i=0x88; break; 				//第三行
		case 0x03: i=0x98; break; 				//第四行
		default : break;
	}
	i = y+i; 									//确定列号
	write_com(i);
}



//********************************************************
//显示字符函数
//********************************************************
void display_char(unsigned char Alphabet)
{
	write_dat(Alphabet);			//写入需要显示字符的显示码
}



//********************************************************
//指定位置显示字符串函数
//参数说明：x为行号，y为列号
//********************************************************
void display_string(unsigned char x,unsigned char y,unsigned char *Alphabet)
{
	unsigned char i=0;
	set_cursor(x,y);				//设置显示的起始地址
	while(Alphabet[i]!='\0')
	{
		write_dat(Alphabet[i]);			//写入需要显示字符的显示码
		i++;
	}
}





//***************************************************************************以下为GDRAM绘图部分************************************************************************//

//*********************绘图显示的清屏函数（因清屏指令在画图时不能用）------------------------------------------------------------------------------注意！！！！！！！
void gui_clear()
{
	uchar i , j , k;
	write_com(EXTEND_SET);//扩展指令集，8位数据传输
	write_com(DRAW_OFF);//绘图显示关闭
	for(i = 0; i < 2; i ++)//分上下两屏写
	{
		for(j = 0; j < 32; j ++)
		{
			write_com(0x80 + j);//写y坐标
			delay(1);
			if(i == 0) //写x坐标
			{
				write_com(0x80);
				delay(1);
			}
			else      //写下半屏
			{
				write_com(0x88);
				delay(1);
			}
			for(k = 0; k < 16; k ++)//写一整行数据
			{
				write_dat(0x00);//写高字节
				write_dat(0x00);//写低字节
				delay(1);
			}
		}
	}
	write_com(DRAW_ON);//打开绘图显示
	write_com(BASIC_SET);//打开基本指令集
}



//*************************************************************************************************
//***************有反白显示功能的打点函数**********************************************************
//参数：color=1，该点填充1；color=0，该点填充白色0；
//*************************************************************************************************
void GUI_Point(unsigned char x,unsigned char y,unsigned char color)
{     
	unsigned char x_Dyte,x_byte;				//定义列地址的字节位，及在字节中的哪1位 
	unsigned char y_Dyte,y_byte;				//定义为上下两个屏(取值为0，1)，行地址(取值为0~31)
	unsigned char GDRAM_hbit,GDRAM_lbit;
	
	write_com(0x36);								//扩展指令命令
	/***X,Y坐标互换，即普通的X,Y坐标***/
	x_Dyte=x/16;								//计算在16个字节中的哪一个
	x_byte=x&0x0f;								//计算在该字节中的哪一位
	y_Dyte=y/32;								//0为上半屏，1为下半屏
	y_byte=y&0x1f;								//计算在0~31当中的哪一行
  
	write_com(0x80+y_byte);						//设定行地址(y坐标),即是垂直地址
	write_com(0x80+x_Dyte+8*y_Dyte);				//设定列地址(x坐标)，并通过8*y_Dyte选定上下屏，即是水平地址
    
	read_dat();									//预读取数据
	GDRAM_hbit= read_dat();						//读取当前显示高8位数据
	GDRAM_lbit= read_dat();						//读取当前显示低8位数据
	delay(1);
  
	write_com(0x80+y_byte);						//设定行地址(y坐标)
	write_com(0x80+x_Dyte+8*y_Dyte);				//设定列地址(x坐标)，并通过8*y_Dyte选定上下屏
	delay(1);
	
	if(x_byte<8)										//判断其在高8位，还是在低8位
	{
		if(color==1)
		{
			write_dat(GDRAM_hbit|(0x01<<(7-x_byte)));	//置位GDRAM区高8位数据中相应的点
		}
		else 
			write_dat(GDRAM_hbit&(~(0x01<<(7-x_byte))));	//清除GDRAM区高8位数据中相应的点	

		write_dat(GDRAM_lbit);							//显示GDRAM区低8位数据 
	}
	else
	{
		write_dat(GDRAM_hbit);
		if(color==1)
			write_dat(GDRAM_lbit|(0x01<<(15-x_byte)));	//置位GDRAM区高8位数据中相应的点
		else 
			write_dat(GDRAM_lbit&(~(0x01<<(15-x_byte))));//清除GDRAM区高8位数据中相应的点	
	} 
	write_com(0x30);										//恢复到基本指令集
}



//***********（给定坐标并打点的）任意位置打点函数
void lcd_set_dot(uchar x,uchar y)
{
	uchar x_byte,x_bit;//确定在坐标的那一字节哪一位
	uchar y_ping , y_bit;//确定在坐标的哪一屏哪一行
	uchar tmph , tmpl;//定义两个临时变量，用于存放读出来的数据
	write_com(EXTEND_SET);//扩展指令集
	write_com(DRAW_OFF);//绘图显示关闭
	x_byte = x / 16;//算出在哪一字节，注意一个地址是16位的
	x_bit = x % 16;//& 0x0f;//算出在哪一位
	y_ping = y / 32;//确定在上半屏还是下半屏，0代表上半屏，1代表下半屏
	y_bit = y % 32;//& 0x1f;//确定在第几行

	write_com(0X80 + y_bit);//先写垂直地址（最高位必须）
	write_com(0x80 + x_byte + 8 * y_ping);//水平坐标，下半屏坐标起始地址为0x88，（+8*y_ping）就是用来确定上半屏还是下半屏
	read_dat();//预读取数据
	tmph = read_dat();//读取当前显示高8位数据
	tmpl = read_dat();//读取当前显示低8位数据
	delay(1);
	write_com(0x80 + y_bit);//读操作会改变AC，所以重新设置一下
	write_com(0x80 + x_byte + 8 * y_ping);
	delay(1);
	if(x_bit < 8)
	{
		write_dat(tmph | (0x01 << (7 - x_bit)));//写高字节，因为坐标是从左向右的，GDRAM高位在昨，低位在右
		write_dat(tmpl);//原低位数据送回
	}
	else
	{
		write_dat(tmph);//原高位数据送回
		write_dat(tmpl | (0x01 << (15 - x_bit)));
	}
	write_com(DRAW_ON);	//打开绘图显示
	write_com(BASIC_SET);//回到基本指令集
}



//************画水平线函数**********************************//
//x0、x1为起始点和终点的水平坐标，y为垂直坐标***************//
//**********************************************************//
void gui_hline(uchar x0, uchar x1, uchar y)
{
	uchar bak;//用于对两个数互换的中间变量，使x1为大值
	if(x0 > x1)
	{
		bak = x1;
		x1 = x0;
		x0 = bak;
	}

	do
	{
		lcd_set_dot(x0 , y);//从左到右逐点显示
		x0 ++;	
	}while(x1 >= x0);
}


//***********画竖直线函数***********************************//
//x为起始点和终点的水平坐标，y0、y1为垂直坐标***************//
//**********************************************************//
void gui_rline(uchar x, uchar y0, uchar y1)
{
	uchar bak;//用于对两个数互换的中间变量，使y1为大值
	if(y0 > y1)
	{
		bak = y1;
		y1 = y0;
		y0 = bak;
	}

	do
	{
		lcd_set_dot(x , y0);//从上到下逐点显示
		y0 ++;	
	}while(y1 >= y0);
}



//*********任意两点间画直线*********************************//
//x0、y0为起始点坐标，x1、y1为终点坐标**********************//
//**********************************************************//
void gui_line(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	char dx;//直线x轴差值
	char dy;//直线y轴差值
	char dx_sym;//x轴增长方向，为-1时减值方向，为1时增值方向
	char dy_sym;//y轴增长方向，为-1时减值方向，为1时增值方向
	char dx_x2;//dx*2值变量，用于加快运算速度
	char dy_x2;//dy*2值变量，用于加快运算速度
	char di;   //决策变量

	if(x0 == x1)//判断是否为垂直线
	{
		gui_rline(x0 , y0 , y1);//画垂直线
		return;
	}
	if(y0 == y1)//判断是否为水平线
	{
		gui_hline(x0 , x1 , y0);//画水平线
		return;
	}

	dx = x1 - x0;//求取两点之间的差值
	dy = y1 - y0;

	//****判断增长方向，或是否为水平线、垂直线、点*//
	if(dx > 0)//判断x轴方向
		dx_sym = 1;
	else
	{
		if(dx < 0)
			dx_sym = -1;
		else
		{
			gui_rline(x0 , y0 , y1);
			return;
		}
	}

	if(dy > 0)//判断y轴方向
		dy_sym = 1;
	else
	{
		if(dy < 0)
			dy_sym = -1;
		else
		{
			gui_hline(x0 , x1 , y0);
			return;
		}
	}


	/*将dx、dy取绝对值***********/
	dx = dx_sym * dx;
	dy = dy_sym * dy;

	/****计算2倍的dx、dy值*******/
	dx_x2 = dx * 1;//我改为了一倍，这样才跟真实的两点对应
	dy_x2 = dy * 1;

	/***使用bresenham法进行画直线***/
	if(dx >= dy)//对于dx>=dy，使用x轴为基准
	{
		di = dy_x2 - dx;
		while(x0 != x1)
		{
			lcd_set_dot(x0,y0);
			x0 +=dx_sym;
			if(di < 0)
				di += dy_x2;//计算出下一步的决策值
			else
			{
				di += dy_x2 - dx_x2;
				y0 += dy_sym;
			}	
		}
		lcd_set_dot(x0, y0);//显示最后一点
	}
	else  //对于dx<dy使用y轴为基准
	{
		di = dx_x2 - dy;
		while(y0 != y1)
		{
			lcd_set_dot(x0, y0);
			y0 += dy_sym;
			if(di < 0)
				di += dx_x2;
			else
			{
				di += dx_x2 - dy_x2;
				x0 += dx_sym;
			}
		}
		lcd_set_dot(x0, y0);//显示最后一点
	}
}



//***************************************************************************//
//*******************画指定宽度的任意两点之间的直线**************************//
//参数说明：x0、y0为起始点坐标，x1、y1为终点坐标，with为线宽*****************//
//***************************************************************************//
void gui_linewith(uchar x0 , uchar y0 , uchar x1 , uchar y1 , uchar with)
{  
	char	dx;						// 直线x轴差值变量
	char	dy;          			// 直线y轴差值变量
	char	dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
	char	dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
	char	dx_x2;					// dx*2值变量，用于加快运算速度
	char	dy_x2;					// dy*2值变量，用于加快运算速度
	char	di;						// 决策变量
   
	char   wx, wy;					// 线宽变量
	char   draw_a, draw_b;
   
	// 参数过滤
	if(with==0) return;
	if(with>50) with = 50;
   
	dx = x1-x0;								// 求取两点之间的差值
	dy = y1-y0;
   
	wx = with/2;
	wy = with-wx-1;
   
	//判断增长方向，或是否为水平线、垂直线、点 
	if(dx>0)								// 判断x轴方向
	{  
		dx_sym = 1;							// dx>0，设置dx_sym=1
	}
	else
	{  
		if(dx<0)
		{  
			dx_sym = -1;					// dx<0，设置dx_sym=-1
		}
		else
		{  
			//dx==0，画垂直线，或一点
			wx = x0-wx;
			if(wx<0) wx = 0;
			wy = x0+wy;
         
			while(1)
			{  
				x0 = wx;
				gui_rline(x0, y0, y1);
				if(wx>=wy) break;
				wx++;
			}
			return;
		}
	}
   
	if(dy>0)								// 判断y轴方向
	{  
		dy_sym = 1;							// dy>0，设置dy_sym=1
	}
	else
	{  
		if(dy<0)
		{  
			dy_sym = -1;					// dy<0，设置dy_sym=-1
		}
		else
		{  
			//dy==0，画水平线，或一点
			wx = y0-wx;
			if(wx<0) wx = 0;
			wy = y0+wy;
			while(1)
			{  
				y0 = wx;
				gui_hline(x0, x1, y1);
				if(wx>=wy) break;
				wx++;
			}
			return;
		}
	}
    
	// 将dx、dy取绝对值
	dx = dx_sym * dx;
	dy = dy_sym * dy;
 
	//计算2倍的dx及dy值
	dx_x2 = dx*2;
	dy_x2 = dy*2;
   
	//使用Bresenham法进行画直线
	if(dx>=dy)								// 对于dx>=dy，则使用x轴为基准
	{  
		di = dy_x2 - dx;
		while(x0!=x1)
		{  
			//x轴向增长，则宽度在y方向，即画垂直线
			draw_a = y0-wx;
			if(draw_a<0) draw_a = 0;
			draw_b = y0+wy;
			gui_rline(x0, draw_a, draw_b);
			x0 += dx_sym;				
			if(di<0)
			{  
				di += dy_x2;				// 计算出下一步的决策值
			}
			else
			{  
				di += dy_x2 - dx_x2;
				y0 += dy_sym;
			}
		}
		draw_a = y0-wx;
		if(draw_a<0) draw_a = 0;
		draw_b = y0+wy;
		gui_rline(x0, draw_a, draw_b);
	}
	else									// 对于dx<dy，则使用y轴为基准
	{  
		di = dx_x2 - dy;
		while(y0!=y1)
		{  
			//y轴向增长，则宽度在x方向，即画水平线
			draw_a = x0-wx;
			if(draw_a<0) draw_a = 0;
			draw_b = x0+wy;
			gui_hline(draw_a, y0, draw_b);
         
			y0 += dy_sym;
			if(di<0)
			{  
				di += dx_x2;
			}
			else
			{  
				di += dx_x2 - dy_x2;
				x0 += dx_sym;
			}
		}
		draw_a = x0-wx;
		if(draw_a<0) draw_a = 0;
		draw_b = x0+wy;
		gui_hline(draw_a, y0, draw_b);
	} 
  
}



//***********画矩形函数*************************************//
//x0、y0为矩形左上角坐标值，x1、y1为矩形右下角坐标值********//
//**********************************************************//
void gui_rectangle(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	gui_hline(x0 , x1 , y0);
	gui_rline(x0 , y0 , y1);
	gui_rline(x1 , y0 , y1);
	gui_hline(x0 , x1 , y1);
}



//****************画填充矩形函数****************************//
//x0、y0为矩形左上角坐标值，x1、y1为矩形右下角坐标值********//
//**********************************************************//
/*void gui_rectangle_fill(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	uchar i;//转换数据的中间变量,使x1、y1大
	if(x0 > x1)
	{
		i = x0;
		x0 = x1;
		x1 = i;
	}
	if(y0 > y1)
	{
		i = y0;
		y0 = y1;
		y1 = i;
	}
	//***判断是否是直线***/
/*	if(y0 == y1)//画水平线
	{
		gui_hline(x0 , x1 , y0);
		return;
	}
	if(x0 == x1)//画竖直线
	{
		gui_rline(x0 , y0 , y1);
		return;
	}

	while(y0 <= y1)//画填充矩形
	{
		gui_hline(x0 , x1 , y0);
		y0 ++;
	}
} */


//*******************画正方形函数*************************//
//x0、y0为正方形左上角坐标，with正方形边长****************//
//********************************************************//
/*void gui_square(uchar x0 , uchar y0 , uchar with)
{
	if(with == 0)
		return;
	if((x0 + with) > 127)//横轴超出液晶边界
		return;
	if((y0 + with) > 63)
		return;
	gui_rectangle(x0 , y0 , x0 + with , y0 + with);
} */



//****************画填充正方形函数*************************//
//x0、y0为正方形左上角坐标，with正方形边长*****************//
//*********************************************************//
/*void gui_square_fill(uchar x0 , uchar y0 , uchar with)
{
	if(with == 0)
		return;
	if((x0 + with) > 127)//横轴超出液晶边界
		return;
	if((y0 + with) > 63)
		return;
	gui_rectangle_fill(x0 , y0 , x0 + with , y0 + with);	
}	*/



//****************画圆函数*********************************//
//x0、y0为圆心坐标，r为圆的半径****************************//
//*********************************************************//
/*void gui_circle(uchar x0 , uchar y0 , uchar r)
{
	char a , b;
	char di;
	if(r > 31 || r == 0)//圆大于液晶屏或者没半径则返回
		return;
	a = 0;
	b = r;
	di = 3 - 2 * r;//判断下个点位置的标志
	while(a <= b)
	{
		lcd_set_dot( x0 - b , y0 - a);//3
		lcd_set_dot( x0 + b , y0 - a); //0
		lcd_set_dot( x0 - a , y0 + b);	//1
		lcd_set_dot( x0 - b , y0 - a); //7
		lcd_set_dot( x0 - a , y0 - b); //2
		lcd_set_dot( x0 + b , y0 + a); //4
		lcd_set_dot( x0 + a , y0 - b); //5
		lcd_set_dot( x0 + a , y0 + b); //6
		lcd_set_dot( x0 - b , y0 + a);
		a ++;
		//***使用bresenham算法画圆********/
/*		if(di < 0)
			di += 4 * a + 6;
		else
		{
			di += 10 + 4 * (a - b);
			b --;
		}	
		lcd_set_dot( x0 + a , y0 + b);
	}
}	*/



//***************************************************************************//
//***************************画正椭圆函数************************************//
//说明：给定椭圆的四个点的参数，最左、最右点的x轴坐标值为x0、x1，最上、最下点
//      的y轴坐标为y0、y1.
//说明：----------------------------显示效果不好
//***************************************************************************//
/*void gui_ellipse(char x0, char x1, char y0, char y1)
{  
	char  draw_x0, draw_y0;			// 刽图点坐标变量
	char  draw_x1, draw_y1;
	char  draw_x2, draw_y2;
	char  draw_x3, draw_y3;
	char  xx, yy;					// 画图控制变量
    
	char  center_x, center_y;		// 椭圆中心点坐标变量
	char  radius_x, radius_y;		// 椭圆的半径，x轴半径和y轴半径
	int  radius_xx, radius_yy;		// 半径乘平方值
	int  radius_xx2, radius_yy2;	// 半径乘平方值的两倍
	char  di;						// 定义决策变量
	
   /* 参数过滤 */
/*  if( (x0==x1) || (y0==y1) ) return; */
   	
   /* 计算出椭圆中心点坐标 */
 /*  center_x = (x0 + x1) >> 1;			
   center_y = (y0 + y1) >> 1;			 */
   
   /* 计算出椭圆的半径，x轴半径和y轴半径 */
  /* if(x0 > x1)
   {  radius_x = (x0 - x1) >> 1;
   }
   else
   {  radius_x = (x1 - x0) >> 1;
   }
   if(y0 > y1)
   {  radius_y = (y0 - y1) >> 1;
   }
   else
   {  radius_y = (y1 - y0) >> 1;
   }		  */
		
   /* 计算半径平方值 */
 /*  radius_xx = radius_x * radius_x;
   radius_yy = radius_y * radius_y;		*/
	
   /* 计算半径平方值乘2值 */
 /*  radius_xx2 = radius_xx<<1;
   radius_yy2 = radius_yy<<1;  */
	
   /* 初始化画图变量 */
 /*  xx = 0;
   yy = radius_y;
  
   di = radius_yy2 + radius_xx - radius_xx2*radius_y ;	// 初始化决策变量 
	
   /* 计算出椭圆y轴上的两个端点坐标，作为作图起点 */
/*   draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;
   draw_y0 = draw_y1 = center_y + radius_y;
   draw_y2 = draw_y3 = center_y - radius_y;
  
	 
   lcd_set_dot(draw_x0, draw_y0);					// 画y轴上的两个端点 
   lcd_set_dot(draw_x2, draw_y2);
	
   while( (radius_yy*xx) < (radius_xx*yy) ) 
   {  if(di<0)
	  {  di+= radius_yy2*(2*xx+3);
	  }
	  else
	  {  di += radius_yy2*(2*xx+3) + 4*radius_xx - 4*radius_xx*yy;
	 	  
	     yy--;
		 draw_y0--;
		 draw_y1--;
		 draw_y2++;
		 draw_y3++;				 
	  }
	  
	  xx ++;						// x轴加1
	 		
	  draw_x0++;
	  draw_x1--;
	  draw_x2++;
	  draw_x3--;
		
	  lcd_set_dot(draw_x0, draw_y0);
	  lcd_set_dot(draw_x1, draw_y1);
	  lcd_set_dot(draw_x2, draw_y2);
	  lcd_set_dot(draw_x3, draw_y3);
   }
  
   di = radius_xx2*(yy-1)*(yy-1) + radius_yy2*xx*xx + radius_yy + radius_yy2*xx - radius_xx2*radius_yy;
   while(yy>=0) 
   {  if(di<0)
	  {  di+= radius_xx2*3 + 4*radius_yy*xx + 4*radius_yy - 2*radius_xx2*yy;
	 	  
	     xx ++;						// x轴加1	 		
	     draw_x0++;
	     draw_x1--;
	     draw_x2++;
	     draw_x3--;  
	  }
	  else
	  {  di += radius_xx2*3 - 2*radius_xx2*yy;	 	 		     			 
	  }
	  
	  yy--;
 	  draw_y0--;
	  draw_y1--;
	  draw_y2++;
	  draw_y3++;	
		
	  lcd_set_dot(draw_x0, draw_y0);
	  lcd_set_dot(draw_x1, draw_y1);
	  lcd_set_dot(draw_x2, draw_y2);
	  lcd_set_dot(draw_x3, draw_y3);
   }     
} */



//*******************************************************************
//*******画满屏图片-----本程序为逐行写，因此图片数据也应该是逐行取的
//参数：dat为填充的数据------用本程序时需要满屏图的数组
//*******************************************************************
/*void gui_draw_full_picture (unsigned char *dat)
{ 
	unsigned char i;                 
	unsigned char j; 
	unsigned char k; 
	unsigned char bGDRAMAddrX = 0x80;		//GDRAM水平地址 
	unsigned char bGDRAMAddrY = 0x80;		//GDRAM垂直地址 
	for(i = 0; i < 2; i++)					                                                                                         
	{ 
		for(j = 0; j < 32; j++)                                                                 
		{ 
			for(k = 0; k < 8; k++)                                                         
			{ 
				write_com(0x34);				//设置为8位MPU接口，扩充指令集，关闭绘图显示
				write_com(bGDRAMAddrY+j);		//垂直地址Y                                                                         
				write_com(bGDRAMAddrX+k);		//水平地址X 
				write_dat(*dat++);			//写数据高字节
				write_dat(*dat++);			//写数据低字节
			} 
		} 
		bGDRAMAddrX = 0x88;					//写下半屏幕
	} 
	write_com(0x36);							//打开绘图模式
	write_com(0x30);							//恢复基本指令集，关闭绘图模式   
} 	*/



unsigned char code  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
//****************************输出一行数据函数，此行可任意长，不必非得是8的倍数**和下个函数合用画任意大小（矩形）的图形或汉字				
//参数：flag反显标志，1为反显 ，x、y为指定显示位置的起始点，*dat要输出的点阵数组，no显示此行所需的点个数，即图形一行的点数
void gui_loadline(unsigned char x,unsigned char y,unsigned char *dat,unsigned char no,unsigned char flag)
{  
	unsigned char bit_dat;
	unsigned char i;

	/* 参数过滤，若指定显示位置超出液晶屏则返回 */
	if(x>127) return;
	if(y>63) return;
   
	for(i=0; i<no; i++)//超出本行所规定的点数则本行显示完成
	{  
		/* 判断是否要读取点阵数据，每字节的开始读取一次点阵数组即i为8的倍数时 */
		if( (i%8)==0 ) bit_dat = *dat++;
     
		/* 对相应的点打1或打0， i&0x07意思是对8求余*/
		if( (bit_dat&DCB2HEX_TAB[i&0x07])==0 )//取出i对应的位，并判断是否为0 
		{
			if(flag==0)				//判断是否反显，该位取出的数据为0，反显要打为1，flag==0代表不反显
				GUI_Point(x,y,0); 	//正常显示0，GUI_Point(x,y,0)代表在x、y处打0
			else
				GUI_Point(x,y,1); 	//将0反显
		}
		else  
		{
			if(flag==0)
				GUI_Point(x,y,1); 
			else
				GUI_Point(x,y,0); 
		}  		     
     
		if( (++x)>127) return;//若显示超出了液晶屏则返回
   }
}

//***************************************************************************
//*****************在自定义大小的区域内画图或画字函数****************************
//参数说明： x、y指定显示区域的起始点坐标
//          dat	要输出显示的图形或汉字点阵数组。
//          hno	要显示区域的长度
//          lno	要显示区域的高度
//          flag反显标志，1为反显
//****************************************************************************
void  GUI_Put_Area(unsigned char x,unsigned char y,unsigned char *dat,unsigned char hno,unsigned char lno,unsigned char flag)
{  
	unsigned char i;

	for(i=0;i<lno;i++)//逐行打点，打点行数要小于区域高度
	{  
		gui_loadline(x,y,dat,hno,flag);	// 打一行数据
		y++;							//使指针指向下一行	
		dat += (hno>>3);				//比如说上一行打了2字节，此处的意思是使数据向后推进2字节，(hno>>3)意思是算出上一行有几个字节数据
		if((hno&0x07)!=0) //hno&0x07意思是对8求余，因一行不满整字节时，上一句的意思就是算出上一行的整字节数，但实际上在取模时不满一个
			dat++;		  //字节也按一个字节取，所以上一句少计算了一个字节，这里加上
	}
}

//*********************************************************************************以上为GDRAM绘图部分************************************************************************//



//**********************************************************************************以下为CGRAM自定义字库部分******************************************************************//
//********************************************************
//设置CGRAM字库
//ST7920 CGRAM（用户自定义图标）空间分布
//空间1地址：40H～4FH共16个地址，一个地址对应两个字节数据;对应调用码:0000H
//空间2地址：50H～5FH共16个地址，一个地址对应两个字节数据;对应调用码:0002H
//空间3地址：60H～6FH共16个地址，一个地址对应两个字节数据;对应调用码:0004H
//空间4地址：70H～7FH共16个地址，一个地址对应两个字节数据;对应调用码:0006H
//参数说明:num为空间编号，取1、2、3、4,CGRAM_ZIKU为地址指针
//********************************************************
/*void SET_CGRAM(unsigned char num,unsigned char *CGRAM_ZIKU)
{
	unsigned char i,add;
	write_com(0x34);					//再次设置为8位并行口，扩展指令集
	write_com(0x02);					//SR=0,允许设置CGRAM地址
	write_com(0x30);					//恢复设置为8位并行口，基本指令集
	add=(num<<4)|0x40;				//计算CGRAM的首地址
	for(i=0;i<16;i++)
	{
		write_com(add+i);				//设置CGRAM的首地址
		write_dat(CGRAM_ZIKU[i*2]);	//写入高8位数据
		write_dat(CGRAM_ZIKU[i*2+1]);//写入低8位数据
	}
}  */

//********************************************************
//指定位置显示CGRAM自造字函数
//参数说明：x为行号，y为列号，num为编号
//********************************************************
/*void display_CGRAM(unsigned char x,unsigned char y,unsigned char num)
{
	set_cursor(x,y);				//设置显示的起始地址
	write_dat(0x00);					//写入需要显示汉字的高八位数据
	write_dat(num*2);				//写入需要显示字符的低八位数据
}

uchar code CGRAM_ZIKU[] = {0X03,0X80,0X00,0X80,0XF8,0X8E,0X23,0X91,0X22,0X21,0X22,0X20,0X23,0XA0,0X20,0X20,
							0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X21,0X20,0X11,0XF8,0X0E,0X00,0X00,0X00,0X00};//I2C

*/
//**************设置CGRAM字库并显示函数
/*void set_and_display()
{
	SET_CGRAM( 1,CGRAM_ZIKU);//把自定义字库写入CGRAM,且写入空间地址1
	display_CGRAM(0 , 1 , 1);//在指定的位置显示空间地址1中自定义的字符	
}  */
//***************************************************************************以上为自定义字库部分**************************************************************//





//****************12864初始化函数
void lcd_init()
{
	write_com(0x30);//基本指令操作,8位并口
	delay(1);
	write_com(0x06);//设置为游标右移，DDRAM地址加一，画面不动
	delay(1);
	write_com(0x0c);//显示开，关光标
	delay(1);
	write_com(0x01);//清除lcd显示内容
	delay(1);
}


//*****************************主函数
void main()
{
	lcd_init();//12864初始化函数
	gui_clear();//画图时清屏函数
	delay(10);
//	set_and_display();//显示自定义字库
//	gui_circle( 86 , 32 , 10); //画圆
	gui_linewith( 5 , 5 , 120 , 60 , 3);//画指定线宽直线
//	gui_ellipse(10 , 120 , 5 , 60);//画正椭圆
	while(1);
}

