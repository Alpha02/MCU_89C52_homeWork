#include "../sameShit.h"
/**********************************************************************************************************************************************************************/
//����˵����������Ϊ12864��st7920����������ֻʵ������򵥵���ʾ����
//�˿����ã�RS��RW��EN�ֱ�ΪP0�ڵ�0��1��2�����ݿ�ΪP2����ȡģ���ȡ��ͼ�λ��ֱ���������ȡ�ģ���Ϊ����������Һ�������д���
/*******************************************************************************************************************************************************/
#include<stdlib.h>	//�ں�rand��������

//**********�궨������ָ��
#define BASIC_SET  0x30
#define EXTEND_SET 0x34
#define DRAW_ON	   0x36
#define DRAW_OFF   0x34

//*************�˿ڶ���
sbit LCD_RS = P0^0;
sbit LCD_RW = P0^1;
sbit LCD_EN = P0^2;

//************��������
//uchar dis1[10];


//****************����ʱ
void delay(uint k)
{
	uint i;
	uchar j;
	for(i = 0; i < k ;i ++)
		for(j = 0; j < 10 ;j ++);
}


//***********ms����ʱ����
/*void delay_1ms(uint x)
{
	uint i,j;
	for(j = 0;j < x; j++)
		for(i = 0;i < 110; i++);
} */


//***********12864дָ���
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


//********12864д���ݺ���
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


//****************��LCD�ж�����
uchar read_dat(void)
{
	uchar temp;
	P2 = 0XFF;	//�ͷ�������
	LCD_RS = 1;	   //����
	LCD_RW = 1;	  // ��ģʽ
	LCD_EN = 1;    //EΪ�ߵ�ƽ���ж����ݻ�ָ��
	delay(1);
	temp = P2;
	LCD_EN = 0;
	return temp; 
}


//********************************************************
//���ù�꣨��ַ������
//����˵����x---Ϊ�кţ�yΪ�к�
//********************************************************
void set_cursor(unsigned char x, unsigned char y)
{
	unsigned char i;
	switch(x)   								//ȷ���к�
	{
		case 0x00: i=0x80; break;				//��һ��
		case 0x01: i=0x90; break; 				//�ڶ���
		case 0x02: i=0x88; break; 				//������
		case 0x03: i=0x98; break; 				//������
		default : break;
	}
	i = y+i; 									//ȷ���к�
	write_com(i);
}



//********************************************************
//��ʾ�ַ�����
//********************************************************
void display_char(unsigned char Alphabet)
{
	write_dat(Alphabet);			//д����Ҫ��ʾ�ַ�����ʾ��
}



//********************************************************
//ָ��λ����ʾ�ַ�������
//����˵����xΪ�кţ�yΪ�к�
//********************************************************
void display_string(unsigned char x,unsigned char y,unsigned char *Alphabet)
{
	unsigned char i=0;
	set_cursor(x,y);				//������ʾ����ʼ��ַ
	while(Alphabet[i]!='\0')
	{
		write_dat(Alphabet[i]);			//д����Ҫ��ʾ�ַ�����ʾ��
		i++;
	}
}





//***************************************************************************����ΪGDRAM��ͼ����************************************************************************//

//*********************��ͼ��ʾ������������������ָ���ڻ�ͼʱ�����ã�------------------------------------------------------------------------------ע�⣡������������
void gui_clear()
{
	uchar i , j , k;
	write_com(EXTEND_SET);//��չָ���8λ���ݴ���
	write_com(DRAW_OFF);//��ͼ��ʾ�ر�
	for(i = 0; i < 2; i ++)//����������д
	{
		for(j = 0; j < 32; j ++)
		{
			write_com(0x80 + j);//дy����
			delay(1);
			if(i == 0) //дx����
			{
				write_com(0x80);
				delay(1);
			}
			else      //д�°���
			{
				write_com(0x88);
				delay(1);
			}
			for(k = 0; k < 16; k ++)//дһ��������
			{
				write_dat(0x00);//д���ֽ�
				write_dat(0x00);//д���ֽ�
				delay(1);
			}
		}
	}
	write_com(DRAW_ON);//�򿪻�ͼ��ʾ
	write_com(BASIC_SET);//�򿪻���ָ�
}



//*************************************************************************************************
//***************�з�����ʾ���ܵĴ�㺯��**********************************************************
//������color=1���õ����1��color=0���õ�����ɫ0��
//*************************************************************************************************
void GUI_Point(unsigned char x,unsigned char y,unsigned char color)
{     
	unsigned char x_Dyte,x_byte;				//�����е�ַ���ֽ�λ�������ֽ��е���1λ 
	unsigned char y_Dyte,y_byte;				//����Ϊ����������(ȡֵΪ0��1)���е�ַ(ȡֵΪ0~31)
	unsigned char GDRAM_hbit,GDRAM_lbit;
	
	write_com(0x36);								//��չָ������
	/***X,Y���껥��������ͨ��X,Y����***/
	x_Dyte=x/16;								//������16���ֽ��е���һ��
	x_byte=x&0x0f;								//�����ڸ��ֽ��е���һλ
	y_Dyte=y/32;								//0Ϊ�ϰ�����1Ϊ�°���
	y_byte=y&0x1f;								//������0~31���е���һ��
  
	write_com(0x80+y_byte);						//�趨�е�ַ(y����),���Ǵ�ֱ��ַ
	write_com(0x80+x_Dyte+8*y_Dyte);				//�趨�е�ַ(x����)����ͨ��8*y_Dyteѡ��������������ˮƽ��ַ
    
	read_dat();									//Ԥ��ȡ����
	GDRAM_hbit= read_dat();						//��ȡ��ǰ��ʾ��8λ����
	GDRAM_lbit= read_dat();						//��ȡ��ǰ��ʾ��8λ����
	delay(1);
  
	write_com(0x80+y_byte);						//�趨�е�ַ(y����)
	write_com(0x80+x_Dyte+8*y_Dyte);				//�趨�е�ַ(x����)����ͨ��8*y_Dyteѡ��������
	delay(1);
	
	if(x_byte<8)										//�ж����ڸ�8λ�������ڵ�8λ
	{
		if(color==1)
		{
			write_dat(GDRAM_hbit|(0x01<<(7-x_byte)));	//��λGDRAM����8λ��������Ӧ�ĵ�
		}
		else 
			write_dat(GDRAM_hbit&(~(0x01<<(7-x_byte))));	//���GDRAM����8λ��������Ӧ�ĵ�	

		write_dat(GDRAM_lbit);							//��ʾGDRAM����8λ���� 
	}
	else
	{
		write_dat(GDRAM_hbit);
		if(color==1)
			write_dat(GDRAM_lbit|(0x01<<(15-x_byte)));	//��λGDRAM����8λ��������Ӧ�ĵ�
		else 
			write_dat(GDRAM_lbit&(~(0x01<<(15-x_byte))));//���GDRAM����8λ��������Ӧ�ĵ�	
	} 
	write_com(0x30);										//�ָ�������ָ�
}



//***********���������겢���ģ�����λ�ô�㺯��
void lcd_set_dot(uchar x,uchar y)
{
	uchar x_byte,x_bit;//ȷ�����������һ�ֽ���һλ
	uchar y_ping , y_bit;//ȷ�����������һ����һ��
	uchar tmph , tmpl;//����������ʱ���������ڴ�Ŷ�����������
	write_com(EXTEND_SET);//��չָ�
	write_com(DRAW_OFF);//��ͼ��ʾ�ر�
	x_byte = x / 16;//�������һ�ֽڣ�ע��һ����ַ��16λ��
	x_bit = x % 16;//& 0x0f;//�������һλ
	y_ping = y / 32;//ȷ�����ϰ��������°�����0�����ϰ�����1�����°���
	y_bit = y % 32;//& 0x1f;//ȷ���ڵڼ���

	write_com(0X80 + y_bit);//��д��ֱ��ַ�����λ���룩
	write_com(0x80 + x_byte + 8 * y_ping);//ˮƽ���꣬�°���������ʼ��ַΪ0x88����+8*y_ping����������ȷ���ϰ��������°���
	read_dat();//Ԥ��ȡ����
	tmph = read_dat();//��ȡ��ǰ��ʾ��8λ����
	tmpl = read_dat();//��ȡ��ǰ��ʾ��8λ����
	delay(1);
	write_com(0x80 + y_bit);//��������ı�AC��������������һ��
	write_com(0x80 + x_byte + 8 * y_ping);
	delay(1);
	if(x_bit < 8)
	{
		write_dat(tmph | (0x01 << (7 - x_bit)));//д���ֽڣ���Ϊ�����Ǵ������ҵģ�GDRAM��λ���򣬵�λ����
		write_dat(tmpl);//ԭ��λ�����ͻ�
	}
	else
	{
		write_dat(tmph);//ԭ��λ�����ͻ�
		write_dat(tmpl | (0x01 << (15 - x_bit)));
	}
	write_com(DRAW_ON);	//�򿪻�ͼ��ʾ
	write_com(BASIC_SET);//�ص�����ָ�
}



//************��ˮƽ�ߺ���**********************************//
//x0��x1Ϊ��ʼ����յ��ˮƽ���꣬yΪ��ֱ����***************//
//**********************************************************//
void gui_hline(uchar x0, uchar x1, uchar y)
{
	uchar bak;//���ڶ��������������м������ʹx1Ϊ��ֵ
	if(x0 > x1)
	{
		bak = x1;
		x1 = x0;
		x0 = bak;
	}

	do
	{
		lcd_set_dot(x0 , y);//�����������ʾ
		x0 ++;	
	}while(x1 >= x0);
}


//***********����ֱ�ߺ���***********************************//
//xΪ��ʼ����յ��ˮƽ���꣬y0��y1Ϊ��ֱ����***************//
//**********************************************************//
void gui_rline(uchar x, uchar y0, uchar y1)
{
	uchar bak;//���ڶ��������������м������ʹy1Ϊ��ֵ
	if(y0 > y1)
	{
		bak = y1;
		y1 = y0;
		y0 = bak;
	}

	do
	{
		lcd_set_dot(x , y0);//���ϵ��������ʾ
		y0 ++;	
	}while(y1 >= y0);
}



//*********��������仭ֱ��*********************************//
//x0��y0Ϊ��ʼ�����꣬x1��y1Ϊ�յ�����**********************//
//**********************************************************//
void gui_line(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	char dx;//ֱ��x���ֵ
	char dy;//ֱ��y���ֵ
	char dx_sym;//x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
	char dy_sym;//y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
	char dx_x2;//dx*2ֵ���������ڼӿ������ٶ�
	char dy_x2;//dy*2ֵ���������ڼӿ������ٶ�
	char di;   //���߱���

	if(x0 == x1)//�ж��Ƿ�Ϊ��ֱ��
	{
		gui_rline(x0 , y0 , y1);//����ֱ��
		return;
	}
	if(y0 == y1)//�ж��Ƿ�Ϊˮƽ��
	{
		gui_hline(x0 , x1 , y0);//��ˮƽ��
		return;
	}

	dx = x1 - x0;//��ȡ����֮��Ĳ�ֵ
	dy = y1 - y0;

	//****�ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ���*//
	if(dx > 0)//�ж�x�᷽��
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

	if(dy > 0)//�ж�y�᷽��
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


	/*��dx��dyȡ����ֵ***********/
	dx = dx_sym * dx;
	dy = dy_sym * dy;

	/****����2����dx��dyֵ*******/
	dx_x2 = dx * 1;//�Ҹ�Ϊ��һ���������Ÿ���ʵ�������Ӧ
	dy_x2 = dy * 1;

	/***ʹ��bresenham�����л�ֱ��***/
	if(dx >= dy)//����dx>=dy��ʹ��x��Ϊ��׼
	{
		di = dy_x2 - dx;
		while(x0 != x1)
		{
			lcd_set_dot(x0,y0);
			x0 +=dx_sym;
			if(di < 0)
				di += dy_x2;//�������һ���ľ���ֵ
			else
			{
				di += dy_x2 - dx_x2;
				y0 += dy_sym;
			}	
		}
		lcd_set_dot(x0, y0);//��ʾ���һ��
	}
	else  //����dx<dyʹ��y��Ϊ��׼
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
		lcd_set_dot(x0, y0);//��ʾ���һ��
	}
}



//***************************************************************************//
//*******************��ָ����ȵ���������֮���ֱ��**************************//
//����˵����x0��y0Ϊ��ʼ�����꣬x1��y1Ϊ�յ����꣬withΪ�߿�*****************//
//***************************************************************************//
void gui_linewith(uchar x0 , uchar y0 , uchar x1 , uchar y1 , uchar with)
{  
	char	dx;						// ֱ��x���ֵ����
	char	dy;          			// ֱ��y���ֵ����
	char	dx_sym;					// x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
	char	dy_sym;					// y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
	char	dx_x2;					// dx*2ֵ���������ڼӿ������ٶ�
	char	dy_x2;					// dy*2ֵ���������ڼӿ������ٶ�
	char	di;						// ���߱���
   
	char   wx, wy;					// �߿����
	char   draw_a, draw_b;
   
	// ��������
	if(with==0) return;
	if(with>50) with = 50;
   
	dx = x1-x0;								// ��ȡ����֮��Ĳ�ֵ
	dy = y1-y0;
   
	wx = with/2;
	wy = with-wx-1;
   
	//�ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ��� 
	if(dx>0)								// �ж�x�᷽��
	{  
		dx_sym = 1;							// dx>0������dx_sym=1
	}
	else
	{  
		if(dx<0)
		{  
			dx_sym = -1;					// dx<0������dx_sym=-1
		}
		else
		{  
			//dx==0������ֱ�ߣ���һ��
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
   
	if(dy>0)								// �ж�y�᷽��
	{  
		dy_sym = 1;							// dy>0������dy_sym=1
	}
	else
	{  
		if(dy<0)
		{  
			dy_sym = -1;					// dy<0������dy_sym=-1
		}
		else
		{  
			//dy==0����ˮƽ�ߣ���һ��
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
    
	// ��dx��dyȡ����ֵ
	dx = dx_sym * dx;
	dy = dy_sym * dy;
 
	//����2����dx��dyֵ
	dx_x2 = dx*2;
	dy_x2 = dy*2;
   
	//ʹ��Bresenham�����л�ֱ��
	if(dx>=dy)								// ����dx>=dy����ʹ��x��Ϊ��׼
	{  
		di = dy_x2 - dx;
		while(x0!=x1)
		{  
			//x����������������y���򣬼�����ֱ��
			draw_a = y0-wx;
			if(draw_a<0) draw_a = 0;
			draw_b = y0+wy;
			gui_rline(x0, draw_a, draw_b);
			x0 += dx_sym;				
			if(di<0)
			{  
				di += dy_x2;				// �������һ���ľ���ֵ
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
	else									// ����dx<dy����ʹ��y��Ϊ��׼
	{  
		di = dx_x2 - dy;
		while(y0!=y1)
		{  
			//y����������������x���򣬼���ˮƽ��
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



//***********�����κ���*************************************//
//x0��y0Ϊ�������Ͻ�����ֵ��x1��y1Ϊ�������½�����ֵ********//
//**********************************************************//
void gui_rectangle(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	gui_hline(x0 , x1 , y0);
	gui_rline(x0 , y0 , y1);
	gui_rline(x1 , y0 , y1);
	gui_hline(x0 , x1 , y1);
}



//****************�������κ���****************************//
//x0��y0Ϊ�������Ͻ�����ֵ��x1��y1Ϊ�������½�����ֵ********//
//**********************************************************//
/*void gui_rectangle_fill(uchar x0 , uchar y0 , uchar x1 , uchar y1)
{
	uchar i;//ת�����ݵ��м����,ʹx1��y1��
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
	//***�ж��Ƿ���ֱ��***/
/*	if(y0 == y1)//��ˮƽ��
	{
		gui_hline(x0 , x1 , y0);
		return;
	}
	if(x0 == x1)//����ֱ��
	{
		gui_rline(x0 , y0 , y1);
		return;
	}

	while(y0 <= y1)//��������
	{
		gui_hline(x0 , x1 , y0);
		y0 ++;
	}
} */


//*******************�������κ���*************************//
//x0��y0Ϊ���������Ͻ����꣬with�����α߳�****************//
//********************************************************//
/*void gui_square(uchar x0 , uchar y0 , uchar with)
{
	if(with == 0)
		return;
	if((x0 + with) > 127)//���ᳬ��Һ���߽�
		return;
	if((y0 + with) > 63)
		return;
	gui_rectangle(x0 , y0 , x0 + with , y0 + with);
} */



//****************����������κ���*************************//
//x0��y0Ϊ���������Ͻ����꣬with�����α߳�*****************//
//*********************************************************//
/*void gui_square_fill(uchar x0 , uchar y0 , uchar with)
{
	if(with == 0)
		return;
	if((x0 + with) > 127)//���ᳬ��Һ���߽�
		return;
	if((y0 + with) > 63)
		return;
	gui_rectangle_fill(x0 , y0 , x0 + with , y0 + with);	
}	*/



//****************��Բ����*********************************//
//x0��y0ΪԲ�����꣬rΪԲ�İ뾶****************************//
//*********************************************************//
/*void gui_circle(uchar x0 , uchar y0 , uchar r)
{
	char a , b;
	char di;
	if(r > 31 || r == 0)//Բ����Һ��������û�뾶�򷵻�
		return;
	a = 0;
	b = r;
	di = 3 - 2 * r;//�ж��¸���λ�õı�־
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
		//***ʹ��bresenham�㷨��Բ********/
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
//***************************������Բ����************************************//
//˵����������Բ���ĸ���Ĳ������������ҵ��x������ֵΪx0��x1�����ϡ����µ�
//      ��y������Ϊy0��y1.
//˵����----------------------------��ʾЧ������
//***************************************************************************//
/*void gui_ellipse(char x0, char x1, char y0, char y1)
{  
	char  draw_x0, draw_y0;			// ��ͼ���������
	char  draw_x1, draw_y1;
	char  draw_x2, draw_y2;
	char  draw_x3, draw_y3;
	char  xx, yy;					// ��ͼ���Ʊ���
    
	char  center_x, center_y;		// ��Բ���ĵ��������
	char  radius_x, radius_y;		// ��Բ�İ뾶��x��뾶��y��뾶
	int  radius_xx, radius_yy;		// �뾶��ƽ��ֵ
	int  radius_xx2, radius_yy2;	// �뾶��ƽ��ֵ������
	char  di;						// ������߱���
	
   /* �������� */
/*  if( (x0==x1) || (y0==y1) ) return; */
   	
   /* �������Բ���ĵ����� */
 /*  center_x = (x0 + x1) >> 1;			
   center_y = (y0 + y1) >> 1;			 */
   
   /* �������Բ�İ뾶��x��뾶��y��뾶 */
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
		
   /* ����뾶ƽ��ֵ */
 /*  radius_xx = radius_x * radius_x;
   radius_yy = radius_y * radius_y;		*/
	
   /* ����뾶ƽ��ֵ��2ֵ */
 /*  radius_xx2 = radius_xx<<1;
   radius_yy2 = radius_yy<<1;  */
	
   /* ��ʼ����ͼ���� */
 /*  xx = 0;
   yy = radius_y;
  
   di = radius_yy2 + radius_xx - radius_xx2*radius_y ;	// ��ʼ�����߱��� 
	
   /* �������Բy���ϵ������˵����꣬��Ϊ��ͼ��� */
/*   draw_x0 = draw_x1 = draw_x2 = draw_x3 = center_x;
   draw_y0 = draw_y1 = center_y + radius_y;
   draw_y2 = draw_y3 = center_y - radius_y;
  
	 
   lcd_set_dot(draw_x0, draw_y0);					// ��y���ϵ������˵� 
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
	  
	  xx ++;						// x���1
	 		
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
	 	  
	     xx ++;						// x���1	 		
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
//*******������ͼƬ-----������Ϊ����д�����ͼƬ����ҲӦ��������ȡ��
//������datΪ��������------�ñ�����ʱ��Ҫ����ͼ������
//*******************************************************************
/*void gui_draw_full_picture (unsigned char *dat)
{ 
	unsigned char i;                 
	unsigned char j; 
	unsigned char k; 
	unsigned char bGDRAMAddrX = 0x80;		//GDRAMˮƽ��ַ 
	unsigned char bGDRAMAddrY = 0x80;		//GDRAM��ֱ��ַ 
	for(i = 0; i < 2; i++)					                                                                                         
	{ 
		for(j = 0; j < 32; j++)                                                                 
		{ 
			for(k = 0; k < 8; k++)                                                         
			{ 
				write_com(0x34);				//����Ϊ8λMPU�ӿڣ�����ָ����رջ�ͼ��ʾ
				write_com(bGDRAMAddrY+j);		//��ֱ��ַY                                                                         
				write_com(bGDRAMAddrX+k);		//ˮƽ��ַX 
				write_dat(*dat++);			//д���ݸ��ֽ�
				write_dat(*dat++);			//д���ݵ��ֽ�
			} 
		} 
		bGDRAMAddrX = 0x88;					//д�°���Ļ
	} 
	write_com(0x36);							//�򿪻�ͼģʽ
	write_com(0x30);							//�ָ�����ָ����رջ�ͼģʽ   
} 	*/



unsigned char code  DCB2HEX_TAB[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
//****************************���һ�����ݺ��������п����ⳤ�����طǵ���8�ı���**���¸��������û������С�����Σ���ͼ�λ���				
//������flag���Ա�־��1Ϊ���� ��x��yΪָ����ʾλ�õ���ʼ�㣬*datҪ����ĵ������飬no��ʾ��������ĵ��������ͼ��һ�еĵ���
void gui_loadline(unsigned char x,unsigned char y,unsigned char *dat,unsigned char no,unsigned char flag)
{  
	unsigned char bit_dat;
	unsigned char i;

	/* �������ˣ���ָ����ʾλ�ó���Һ�����򷵻� */
	if(x>127) return;
	if(y>63) return;
   
	for(i=0; i<no; i++)//�����������涨�ĵ���������ʾ���
	{  
		/* �ж��Ƿ�Ҫ��ȡ�������ݣ�ÿ�ֽڵĿ�ʼ��ȡһ�ε������鼴iΪ8�ı���ʱ */
		if( (i%8)==0 ) bit_dat = *dat++;
     
		/* ����Ӧ�ĵ��1���0�� i&0x07��˼�Ƕ�8����*/
		if( (bit_dat&DCB2HEX_TAB[i&0x07])==0 )//ȡ��i��Ӧ��λ�����ж��Ƿ�Ϊ0 
		{
			if(flag==0)				//�ж��Ƿ��ԣ���λȡ��������Ϊ0������Ҫ��Ϊ1��flag==0��������
				GUI_Point(x,y,0); 	//������ʾ0��GUI_Point(x,y,0)������x��y����0
			else
				GUI_Point(x,y,1); 	//��0����
		}
		else  
		{
			if(flag==0)
				GUI_Point(x,y,1); 
			else
				GUI_Point(x,y,0); 
		}  		     
     
		if( (++x)>127) return;//����ʾ������Һ�����򷵻�
   }
}

//***************************************************************************
//*****************���Զ����С�������ڻ�ͼ���ֺ���****************************
//����˵���� x��yָ����ʾ�������ʼ������
//          dat	Ҫ�����ʾ��ͼ�λ��ֵ������顣
//          hno	Ҫ��ʾ����ĳ���
//          lno	Ҫ��ʾ����ĸ߶�
//          flag���Ա�־��1Ϊ����
//****************************************************************************
void  GUI_Put_Area(unsigned char x,unsigned char y,unsigned char *dat,unsigned char hno,unsigned char lno,unsigned char flag)
{  
	unsigned char i;

	for(i=0;i<lno;i++)//���д�㣬�������ҪС������߶�
	{  
		gui_loadline(x,y,dat,hno,flag);	// ��һ������
		y++;							//ʹָ��ָ����һ��	
		dat += (hno>>3);				//����˵��һ�д���2�ֽڣ��˴�����˼��ʹ��������ƽ�2�ֽڣ�(hno>>3)��˼�������һ���м����ֽ�����
		if((hno&0x07)!=0) //hno&0x07��˼�Ƕ�8���࣬��һ�в������ֽ�ʱ����һ�����˼���������һ�е����ֽ�������ʵ������ȡģʱ����һ��
			dat++;		  //�ֽ�Ҳ��һ���ֽ�ȡ��������һ���ټ�����һ���ֽڣ��������
	}
}

//*********************************************************************************����ΪGDRAM��ͼ����************************************************************************//



//**********************************************************************************����ΪCGRAM�Զ����ֿⲿ��******************************************************************//
//********************************************************
//����CGRAM�ֿ�
//ST7920 CGRAM���û��Զ���ͼ�꣩�ռ�ֲ�
//�ռ�1��ַ��40H��4FH��16����ַ��һ����ַ��Ӧ�����ֽ�����;��Ӧ������:0000H
//�ռ�2��ַ��50H��5FH��16����ַ��һ����ַ��Ӧ�����ֽ�����;��Ӧ������:0002H
//�ռ�3��ַ��60H��6FH��16����ַ��һ����ַ��Ӧ�����ֽ�����;��Ӧ������:0004H
//�ռ�4��ַ��70H��7FH��16����ַ��һ����ַ��Ӧ�����ֽ�����;��Ӧ������:0006H
//����˵��:numΪ�ռ��ţ�ȡ1��2��3��4,CGRAM_ZIKUΪ��ַָ��
//********************************************************
/*void SET_CGRAM(unsigned char num,unsigned char *CGRAM_ZIKU)
{
	unsigned char i,add;
	write_com(0x34);					//�ٴ�����Ϊ8λ���пڣ���չָ�
	write_com(0x02);					//SR=0,��������CGRAM��ַ
	write_com(0x30);					//�ָ�����Ϊ8λ���пڣ�����ָ�
	add=(num<<4)|0x40;				//����CGRAM���׵�ַ
	for(i=0;i<16;i++)
	{
		write_com(add+i);				//����CGRAM���׵�ַ
		write_dat(CGRAM_ZIKU[i*2]);	//д���8λ����
		write_dat(CGRAM_ZIKU[i*2+1]);//д���8λ����
	}
}  */

//********************************************************
//ָ��λ����ʾCGRAM�����ֺ���
//����˵����xΪ�кţ�yΪ�кţ�numΪ���
//********************************************************
/*void display_CGRAM(unsigned char x,unsigned char y,unsigned char num)
{
	set_cursor(x,y);				//������ʾ����ʼ��ַ
	write_dat(0x00);					//д����Ҫ��ʾ���ֵĸ߰�λ����
	write_dat(num*2);				//д����Ҫ��ʾ�ַ��ĵͰ�λ����
}

uchar code CGRAM_ZIKU[] = {0X03,0X80,0X00,0X80,0XF8,0X8E,0X23,0X91,0X22,0X21,0X22,0X20,0X23,0XA0,0X20,0X20,
							0X20,0X20,0X20,0X20,0X20,0X20,0X20,0X21,0X20,0X11,0XF8,0X0E,0X00,0X00,0X00,0X00};//I2C

*/
//**************����CGRAM�ֿⲢ��ʾ����
/*void set_and_display()
{
	SET_CGRAM( 1,CGRAM_ZIKU);//���Զ����ֿ�д��CGRAM,��д��ռ��ַ1
	display_CGRAM(0 , 1 , 1);//��ָ����λ����ʾ�ռ��ַ1���Զ�����ַ�	
}  */
//***************************************************************************����Ϊ�Զ����ֿⲿ��**************************************************************//





//****************12864��ʼ������
void lcd_init()
{
	write_com(0x30);//����ָ�����,8λ����
	delay(1);
	write_com(0x06);//����Ϊ�α����ƣ�DDRAM��ַ��һ�����治��
	delay(1);
	write_com(0x0c);//��ʾ�����ع��
	delay(1);
	write_com(0x01);//���lcd��ʾ����
	delay(1);
}


//*****************************������
void main()
{
	lcd_init();//12864��ʼ������
	gui_clear();//��ͼʱ��������
	delay(10);
//	set_and_display();//��ʾ�Զ����ֿ�
//	gui_circle( 86 , 32 , 10); //��Բ
	gui_linewith( 5 , 5 , 120 , 60 , 3);//��ָ���߿�ֱ��
//	gui_ellipse(10 , 120 , 5 , 60);//������Բ
	while(1);
}

