#include "main.h"
#define ID1_BUF_LEN 8

#define autoModeButton PAin(0)      //�Զ���ʻģʽ�л�����
#define emerStopButton PAin(1)      //���������ƶ�����
#define wirelessStopButton PCin(0)  //���߽����ƶ�����
#define stateLED1  PAout(4)
#define stateLED2  PAout(5)

#define emergencyBrakeLED LED0
#define driverlessModeLED LED1
#define LED_OPEN 0
#define LED_CLOSE 1

#define noBraking 1500               //���ƶ�CCR(�ȽϼĴ���)ֵ
#define maxBraking 2200              //����ƶ�CCRֵ
#define cheatBrakingVal 26 // waiting to be tested
u8 g_brakingVal = 0;
u8 g_isDriverless = 0;

/*@brief д����
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

/*@brief ����У��ֵ(�ۼӺ�У��)
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

/*@brief ϵͳ��ʼ��
*/
void system_init()
{
	delay_init();	//��ʱ���ܳ�ʼ�� 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�ж����ȼ�����
	
	//CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_Normal);
	uart1_init(115200);	//����1��ʼ��
	//Adc_Init();
 	LED_Init();         //LED��ʼ��
	BEEP_Init();        //��������ʼ��
	KEY_Init();         //������ʼ��
	TIM3_PWM_Init();    //TIM3PWM��ʼ�������ƶ���ƶ�
	//IWDG_Init(0,700); //���Ź�
  
	//printf("��ʼ�����...\r\n");
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
	pkgID1.driverlessMode = 0;
	pkgID1.emergencyBrake = 0;

	system_init();
	delay_ms(100);
	TIM_SetCompare1(TIM3,noBraking); //��ʼ��Ϊ���ƶ�
		
	while(1)
	{
		if (emerStopButton==1 || wirelessStopButton==1)//��ͣ���ذ���
		{
			pkgID1.emergencyBrake = 1;
			emergencyBrakeLED = LED_OPEN;
			expectBrakeCCR = maxBraking;
		}
		else
		{
			pkgID1.emergencyBrake = 0;
			emergencyBrakeLED = LED_CLOSE;
			expectBrakeCCR = noBraking;
		}
		
		if (autoModeButton==1)//�Զ���ʻģʽ���رպ�
		{
			pkgID1.driverlessMode = 1;
			driverlessModeLED = LED_OPEN;
			uint16_temp = noBraking+(maxBraking-noBraking)/1000*g_brakingVal; //���ƶ�������pwm CCR
			expectBrakeCCR = uint16_temp > expectBrakeCCR ? uint16_temp : expectBrakeCCR; //ȡ���ֵ
		}
		else
		{
			pkgID1.driverlessMode = 0;
			driverlessModeLED = LED_CLOSE;
			expectBrakeCCR = noBraking;
		}
		TIM_SetCompare1(TIM3,expectBrakeCCR); //�����ƶ��Ƚ�ֵ,�����ƶ�
		
		//�Զ���ʻ״̬��
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
		
		//����У���
		pkgID1.checkSum = generate_check_sum((uint8_t*)&pkgID1, ID1_BUF_LEN-1);
		//��������
		write(USART1, (uint8_t*)&pkgID1, ID1_BUF_LEN);

		delay_ms(20);
	}
	return 0;
}