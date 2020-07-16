#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

//���ء���
// PA0 �Զ���ʻģʽ��ť��Ĭ������
// PA1 ��������ƶ���Ĭ������
// PC0 ���߽����ƶ���Ĭ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTA,PORTEʱ��

	//��ʼ�� GPIOA.0 GPIOA.1	  ��������
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���룬Ĭ������	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//��ʼ��GPIOC.0
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���룬Ĭ������	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}
