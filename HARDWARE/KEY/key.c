#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"

//开关——
// PA0 自动驾驶模式按钮，默认下拉
// PA1 车身紧急制动，默认下拉
// PC0 无线紧急制动，默认下拉
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB,ENABLE);//使能PORTA,PORTE时钟

	//初始化 GPIOA.0 GPIOA.1	  下拉输入
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //输入，默认下拉	  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//初始化GPIOC.0
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //输入，默认下拉	  
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}
