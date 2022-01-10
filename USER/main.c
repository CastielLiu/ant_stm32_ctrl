#include "main.h"
#define ID1_BUF_LEN 8

#define autoModeButton PAin(0)      //自动驾驶模式切换引脚
#define emerStopButton PAin(1)      //车身紧急制动引脚
#define wirelessStopButton PCin(0)  //无线紧急制动引脚
#define stateLED1  PAout(4)
#define stateLED2  PAout(5)

#define emergencyBrakeLED LED0
#define driverlessModeLED LED1
#define LED_OPEN 0
#define LED_CLOSE 1

#define noBraking 1500               //无制动CCR(比较寄存器)值
#define maxBraking 2200              //最大制动CCR值

//全局变量
//由串口中断程序更改其值
u8 g_brakingVal = 0;   //0-100
u8 g_isDriverless = 0;

/*@brief 写串口
*/
void write(USART_TypeDef* USARTx, uint8_t *Data,uint16_t len)
{
	int i=0;
	for(;i<len;i++)
	{
		USART_SendData(USARTx,*(Data+i));
		while(!USART_GetFlagStatus(USARTx,USART_FLAG_TXE));
	}
}

/*@brief 生成校验值(累加和校验)
*/
uint8_t generate_check_sum(uint8_t *buf,int len)
{
	uint8_t sum=0;
	int i=2;
	for(;i<len;i++)
	{
		sum += *(buf+i);
	}
	return sum;
}

/*@brief 系统初始化
*/
void system_init()
{
	delay_init();	//延时功能初始化 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级配置
	
	//CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);
	uart1_init(115200);	//串口1初始化
	//Adc_Init();
 	LED_Init();         //LED初始化
	BEEP_Init();        //蜂鸣器初始化
	KEY_Init();         //按键初始化
	TIM3_PWM_Init();    //TIM3PWM初始化，控制舵机制动
	TIM3->CCR1 = noBraking; 
	//IWDG_Init(0,700); //看门狗
  
	printf("初始化完成...\r\n");
}

int main(void)
{	
	pkg_id1_t pkgID1;
	uint16_t uint16_temp = 0;
	//uint8_t uint8_temp = 0;
	uint16_t expectBrakeCCR = noBraking;
	
	pkgID1.header[0] = 0x66;
	pkgID1.header[1] = 0xcc;
	pkgID1.pkgLen_H  = 0;
	pkgID1.pkgLen_L  = 4;
	pkgID1.pkgID     = 0x01;
	pkgID1.driverlessKey = 0;
	pkgID1.emergencyBrake = 0;

	system_init();
	delay_ms(100);
	TIM_SetCompare1(TIM3,noBraking); //初始化为无制动
		
	while(1)
	{
		if (autoModeButton==1)//自动驾驶模式开关闭合
		{
			pkgID1.driverlessKey = 1;    //自动驾驶请求置位
			driverlessModeLED = LED_OPEN; //自动驾驶状态灯open
			expectBrakeCCR = noBraking+ 1.0*(maxBraking-noBraking)/100*g_brakingVal; //由制动请求换算pwm CCR
		}
		else
		{
			pkgID1.driverlessKey = 0;    //自动驾驶模式复位
			driverlessModeLED = LED_CLOSE;//自动驾驶状态灯close
			expectBrakeCCR = noBraking;
		}
		
		if (emerStopButton==1 || wirelessStopButton==1)//急停开关按下
		{
			pkgID1.emergencyBrake = 1;     //急停请求置位
			emergencyBrakeLED = LED_OPEN;  //开启急停指示灯
			expectBrakeCCR = maxBraking;   //最大制动力
		}
		else //非急停状态
		{
			pkgID1.emergencyBrake = 0;     //急停请求复位
			emergencyBrakeLED = LED_CLOSE; //关闭急停指示灯
			
			//与之前的期望值比较，若前方期望值较高，以前方值为准
			expectBrakeCCR = expectBrakeCCR > noBraking ? expectBrakeCCR : noBraking;
		}
		
		TIM_SetCompare1(TIM3,expectBrakeCCR); //设置制动比较值,触发制动
		
		//printf("expectBrake: %d\r\n", expectBrakeCCR);
		//printf("%d\t %d\r\n",g_brakingVal, g_isDriverless);
		
		//自动驾驶状态灯
		if(g_isDriverless == 0)
		{
			stateLED1 = LED_OPEN;
			stateLED2 = LED_CLOSE;
		}
		else
		{
			stateLED1 = LED_CLOSE;
			stateLED2 = LED_OPEN;
		}
		
		//生成校验和
		pkgID1.checkSum = generate_check_sum((uint8_t*)&pkgID1, ID1_BUF_LEN-1);
		//发送数据
		write(USART1, (uint8_t*)&pkgID1, ID1_BUF_LEN);
		

		delay_ms(20);
	}
	return 0;
}
