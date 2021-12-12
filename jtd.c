#include <reg52.h>
#include <intrins.h>
#define	uint unsigned int
#define uchar unsigned char
uchar night_flag=0;				//深夜标志位
uchar time_cl=0;				//定义时间控制标志位  取0	
uchar flag=0;					//方向通行标志位
uchar B_N_D_flag=1;				//定义北南通灯行标志位
uchar X_D_D_flag=1;				//定义西东通灯行标志位
uchar i,j;						//定义变量i
uchar t=3;						//数码管动态扫描延时时间基数
uchar num=0;					//时间基数初始化为0
//uchar num_night=0;				//深夜模式时间基数为0
uchar time=30;					//time初始化定为30秒
int bei,nan,xi,dong;            //定义  北  南  西  东
uchar code Data[]={0x18,0x7B,0x89,0x49,0x6A,0x4C,0x0C,0x79,0x08,0x48,0xEF};
//定义数组  数码管   0    1    2    3    4    5    6    7    8    9    -
/***************定义  红黄绿灯**************/
sbit    B_R=P1^2;				//北面  红灯
sbit    B_Y=P1^1;				//北面  黄灯
sbit    B_G=P1^0;				//北面	绿灯
sbit    N_R=P2^0;				//南面	红灯
sbit    N_Y=P2^1;				//南面	黄灯
sbit	N_G=P2^2;				//南面	绿灯
sbit    X_R=P2^5;//西面	红灯
sbit    X_Y=P2^6;				//西面	黄灯
sbit    X_G=P2^7;				//西面	绿灯

sbit    D_R=P3^7;				//东面  红灯
sbit    D_Y=P3^6;				//东面	黄灯
sbit    D_G=P3^5;				//东面  绿灯

sbit    s1=P3^4;//时间减
sbit    s2=P3^3;//时间加
sbit    s3=P3^1;//白天和夜间模式控制切换口
sbit    s4=P3^2;//进入时间可以设置状态的切换口
/***************定义  数码管*****************/
#define	SMG_XS	P0				//数码管数据显示
#define	SMG_KZ	P2				//数码管控制显示
sbit   smg1=P2^4;//定义数码管1控制口
sbit   smg2=P2^3;//定义数码管2控制口
void delay(uint time);// 定义延时程序
void disp_init();//显示初始化
void disp(bei,nan,xi,dong);




void Time0_init()				//定时器0初始化函数
{
	TMOD=0x01;					//定时器0为方式1
	TL0=(65536-50000)%256;
	TH0=(65536-50000)/256;		//12M晶振 定时时间50ms
	ET0=1;						//开定时器0中断
	EA=1;						//开总中断
	TR0=1;						//启动定时器0
}

void night_mode()				//深夜模式
{
	B_R=1;
	B_Y=~B_Y;					//北 黄灯取反
	B_G=1;
	N_R=1;
	N_Y=~N_Y;					//南 黄灯取反
	N_G=1;

	X_R=1;					
	X_Y=~X_Y;					//西 黄灯取反
	X_G=1;
	D_R=1;
	D_Y=~D_Y;					//东 黄灯取反
	D_G=1;	
}

void B_N_G()					//北南通(绿灯)  西东(红灯)			
{
	B_G=0;						//北  绿灯亮
	N_G=0;						//南  绿灯亮
	B_R=1;
	B_Y=1;
	N_R=1;
	N_Y=1;

	X_R=0;						//西  红灯亮
	D_R=0;						//东  红灯亮
	X_Y=1;
	X_G=1;
	D_Y=1;
	D_G=1;
}

void X_D_G()					//西东通(绿灯)  北南(红灯)			
{
	B_G=1;
	N_G=1;
	B_R=0;						//北  红灯亮
	B_Y=1;
	N_R=0;						//南  红灯亮
	N_Y=1;

	X_R=1;
	D_R=1;
	X_Y=1;
	X_G=0;						//西  绿灯亮
	D_Y=1;
	D_G=0;						//东  绿灯亮
}

void B_N_tong()					//北南 通行
{
	if(B_N_D_flag==1)			//当北南通灯行标志位为1
	{
		B_N_G();				//北南通(绿灯)  西东(红灯)
		B_N_D_flag=0;			//北南通灯行标志位置0
	}
	while(num==20)				//当1s时间到
	{
		num=0;					//num清0
		bei--;					//北时间 减1
		nan--;					//南时间 减1
		xi--;					//西时间 减1
		dong--;					//东时间 减1
		if(bei<=6&&bei>=0||nan<=6&&nan>=0)		//当北和南时间在0到6秒之间
		{
			if(bei<=6&&bei>3||nan<=6&&nan>3)	//进一步判断北和南时间是否在3到6秒之间
			{
				B_G=~B_G;						//北  绿灯取反
				N_G=~N_G;						//南  绿灯取反
			}
			if(bei<=3&&nan<=3)					//进一步判断北和南时间是否在3秒以内
			{
				B_G=1;							//北  绿灯不亮
				N_G=1;							//南  绿灯不亮
				B_Y=0;							//北  黄灯亮
				N_Y=0;							//南  黄灯亮
			}
		}
		else									//否则(即北和南时间不在0到6秒之间)
		{
			B_G=0;								//北  绿灯亮
			N_G=0;								//南  绿灯亮
			B_Y=1;								//北  黄灯不亮
			N_Y=1;								//南  黄灯不亮
		}
	}
}

void X_D_tong()									//西东  通行
{
	if(X_D_D_flag==1)							//当西东通灯行标志位为1
	{
		X_D_G();								//西东通(绿灯)  北南(红灯)
		X_D_D_flag=0;							//西东通灯行标志位置0
	}
	while(num==20)								//当1s时间到
	{
		num=0;									//num清0
		bei--;									//北时间 减1
		nan--;									//南时间 减1		
		xi--;									//西时间 减1
		dong--;									//东时间 减1	
		if(xi<=6&&xi>=0||dong<=6&&xi>=0)		//当西和东时间在0到6秒之间
		{
			if(xi<=6&&xi>3||dong<=6&&dong>3)	//进一步判断西和东时间是否在3到6秒之间
			{
				X_G=~X_G;						//西  绿灯取反
				D_G=~D_G;						//东  绿灯取反
			}
			if(xi<=3&&dong<=3)					//进一步判断西和东时间是否在3秒以内
			{
				X_G=1;							//西  绿灯不亮
				D_G=1;							//东  绿灯不亮
				X_Y=0;							//西  黄灯亮
				D_Y=0;							//东  黄灯亮
			}
		}
		else									//否则(即西和东时间不在0到6秒之间)
		{
			X_G=0;								//西  绿灯亮
			D_G=0;								//东  绿灯亮
			X_Y=1;								//西  黄灯不亮
			D_Y=1;//东  黄灯不亮	
		}
	}		
}

void Time0() interrupt 1						//定时器0中断服务函数
{
	TH0=(65536-50000)/256;						//重新载装初值,设置50ms中断一次
	TL0=(65536-50000)%256;
	num++;										//时间基数加1
}
		
void main()										//主函数入口											
{
	bei=nan=xi=dong=time;						//把time值赋给北 南 西 东
	disp_init();			//显示初始化
	Time0_init();		//定时器0初始化函数		
	while(1)		//无限循环
	{  
		if(s4==0)								//当P3.0按下(调整时间允许键)
		{
			delay(5);							//延时去抖
			s4=1;						//关闭数码管显示
			if(s4==0)		    				//再次确定按键是否按下
			{
				time_cl=~time_cl;//时间控制标志位取反(开/关时间控制标志位)	
				if(time_cl==0)					//如果时间控制标志位为0
				{
					TL0=(65536-50000)%256;
					TH0=(65536-50000)/256;		//12M晶振 定时时间50ms
					TR0=1;						//开启定时器0
					num=0;						//num初始化为0	
				}	
			}	
			while(s4==0);						//等待按键松开
		}
		
		if(s3==0)								//当P3.7按下(开/关 深夜模式)
		{
			delay(5);							//延时去抖
		    s3=1;						//关闭数码管显示
			if(s3==0)
			{
				night_flag=~night_flag;			//深夜标志位取反(开/关深夜模式)，1表示开，0表示关
				if(night_flag==1)				//如果深夜模式开
				{
					TR0=0;						//关闭定时器0
					num=0;						//时间基数清0
					TL0=(65536-50000)%256;
					TH0=(65536-50000)/256;		//12M晶振 定时时间50ms
					TR0=1;						//开启定时器0
					SMG_KZ=0xff;				//关闭数码管显示	
				}
				else 							//否则night_flag=0表示深夜模式关(红绿灯正常运行)					
				{
					TR0=0;						//关闭定时器0
					num=0;						//时间基数清0
					TL0=(65536-50000)%256;
					TH0=(65536-50000)/256;		//12M晶振 定时时间50ms
					TR0=1;						//开启定时器0
					B_N_D_flag=1;				//定义北南通灯行标志位
					X_D_D_flag=1;				//定义西东通灯行标志位	
					if(bei<=3&&nan<=3)			//判断北和南时间是否在3秒以内
					{
						B_Y=1;
						N_Y=1;
					}
					if(xi<=3&&dong<=3)			//判断西和东时间是否在3秒以内
					{
						X_Y=1;
						D_Y=1;
					}				
				}
			}
			while(s3==0);									
		}
		if(night_flag==0)						//如果深夜标志位为0，表示关闭深夜模式，红绿灯正常运行
		{
			if(time_cl==0)							//如果time_cl=0(不允许调整时间)，即红绿灯正常运行
			{	
				if(flag==0)							//flag=0表示北南通行允许
				{
					B_N_tong();						//北南 通行
					if(bei==0)						//北南通行时间到
					{
						flag=~flag;					//西东 通行允许
						B_N_D_flag=1;				//北南通灯行标志位置1
					}
				}
				else								//flag=1表示西东通行允许
				{
					X_D_tong();						//西东 通行 
					if(xi==0)						//西东通行时间到
					{
						flag=~flag;					//北南 通行允许
						X_D_D_flag=1;				//西东通灯行标志位置1
					}
				}
	
				disp(bei,nan,xi,dong);				//显示时间数据
				if(bei==0&&nan==0&&xi==0&&dong==0)	//如果 北 南 西 东时间到(为0)
				{
					TR0=0;							//关闭定时器0
					TL0=(65536-50000)%256;			
					TH0=(65536-50000)/256;			//12M晶振 定时时间50ms
					for(j=0;j<3;j++)				//延时显示闪烁3下
					{
						for(i=0;i<10;i++)			//for语句延时
						{
							disp(bei,nan,xi,dong);  //显示函数
						
						}
						SMG_KZ=0xff;				//关闭显示
						delay(200);					//延时一下
					}
					bei=time;						//time值赋给北
					nan=time;						//time值赋给南
					xi=time;						//time值赋给西
					dong=time;						//time值赋给东
					TR0=1;							//开启定时器0
				}
				else								//否则
				{}									//空操作
			}
			else									//如果time_cl==1(允许时间调整)
			{
				TR0=0;								//关闭定时器
				TL0=(65536-50000)%256;
				TH0=(65536-50000)/256;				//12M晶振 定时时间50ms
				num=0;								//时间基数清0
				bei=nan=xi=dong=time;				//time值赋给 北 南 西 东
				disp(bei,nan,xi,dong);				//显示时间数据
				if(s2==0)							//当P3.1按键按下(操作时间加1)
				{
					delay(5);						//延时去抖
					if(s2==0)						//再次确定按键是否按下
					{
						time++;						//时间加1
					}	
					while(s2==0);					//等待按键松开	
				}
				if(bei==100&&nan==100&&xi==100&&dong==100)	//如果北 南 西 东 时间到100
				{	
					time=1;							//time等于1
				}
				if(s1==0)							//当P3.6按键按下(操作时间减1)		
				{										
					delay(5);						//延时去抖
					if(s1==0)						//再次确定按键是否按下
					{
						time--;						//time减1
					}
					while(s1==0);					//等待按键松开
				}
				if(bei==0&&nan==0&&xi==0&&dong==0)	//如果北 南 西 东 时间到0
				{
					time=99;						//time等于99
				}	
			}
		}
		else										//否则深夜标志位为1，表示开启深夜模式
		{		
			if(num==20)								//当1s时间到
			{
				num=0;								//50ms时间基数清0
				night_mode();						//深夜模式
			}				
		}
  //  disp(69,69,69,69);			
	}
    
}






void disp(bei,nan,xi,dong) 		//显示(北 南 西 东)时间函数
{
	SMG_XS=Data[bei/10];		//显示北的十位
	smg1=0;				//允许北十位显示
	delay(t);					//小延时一下,使数码管显示更清楚
    smg1=1;			//关闭数码管显示(消隐)
	SMG_XS=Data[bei%10];		//显示北的个位
	smg2=0;			//允许北个位显示
    delay(t);					//小延时一下,使数码管显示更清楚
	smg2=1;			//关闭数码管显示(消隐)	
	SMG_XS=Data[nan/10];		//显示南的十位	
//SMG_KZ=0xf7;				//允许南十位显示
    smg1=0;
  	delay(t);					//小延时一下,使数码管显示更清楚
	smg1=1;
    //SMG_KZ=0xff;				//关闭数码管显示(消隐)
	SMG_XS=Data[nan%10];		//显示南的个位
	//SMG_KZ=0xfb;				//允许南个位显示
    smg2=0;	
    delay(t);					//小延时一下,使数码管显示更清楚
	//SMG_KZ=0xff;				//关闭数码管显示(消隐)
    smg2=1;
	SMG_XS=Data[xi/10];			//显示西的十位	
  //	SMG_KZ=0xdf;				//允许西十位显示
	smg1=0;
    delay(t);					//小延时一下,使数码管显示更清楚
	smg1=1;
    //SMG_KZ=0xff;				//关闭数码管显示(消隐)
	SMG_XS=Data[xi%10];			//显示西的个位
	smg2=0;
    //SMG_KZ=0xef;				//允许西个位显示
	delay(t);					//小延时一下,使数码管显示更清楚
	smg2=1;
    //SMG_KZ=0xff;				//关闭数码管显示(消隐)

	SMG_XS=Data[dong/10];		//显示东的十位		
	smg1=0;
    //SMG_KZ=0x7f;				//允许东十位显示
	delay(t);					//小延时一下,使数码管显示更清楚
	smg1=1;
    //SMG_KZ=0xff;				//关闭数码管显示(消隐)
	SMG_XS=Data[dong%10];		//显示东的个位
	smg2=0;//
    //SMG_KZ=0xbf;				//允许东个位显示
	delay(t);					//小延时一下,使数码管显示更清楚
	smg2=1;
    //SMG_KZ=0xff;				//关闭数码管显示(消隐)
}
void disp_init()				//显示初始化
{
	for(i=0;i<3;i++)
	{
		SMG_XS=Data[10];		//数码管显示"--"
        smg1=0;
        smg2=0;//允许数码管显示
        B_R=0;				//北面  红灯
        B_Y=0;				//北面  黄灯
        B_G=0;				//北面	绿灯
        N_R=0;				//南面	红灯
        N_Y=0;				//南面	黄灯
	    N_G=0;				//南面	绿灯
        X_R=0;//西面	红灯
        X_Y=0;				//西面	黄灯
        X_G=0;				//西面	绿灯
        D_R=0;				//东面  红灯
        D_Y=0;				//东面	黄灯
        D_G=0;				//东面  绿灯
		delay(300);				//延时
        B_R=1;				//北面  红灯
        B_Y=1;				//北面  黄灯
        B_G=1;				//北面	绿灯
        N_R=1;				//南面	红灯
        N_Y=1;				//南面	黄灯
	    N_G=1;				//南面	绿灯
        X_R=1;//西面	红灯
        X_Y=1;				//西面	黄灯
        X_G=1;				//西面	绿灯
        D_R=1;				//东面  红灯
        D_Y=1;				//东面	黄灯
        D_G=1;				//东面  绿灯
		delay(300);				//延时
	}
}
void delay(uint time)			//扫描延时函数
{
	uint x,y;
	for(x=time;x>0;x--)
	for(y=110;y>0;y--);		
}

