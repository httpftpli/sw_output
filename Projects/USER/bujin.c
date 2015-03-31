#include "bujin.h"


const GPIOGROUP  MiduMotoFree[4]={
 	{Moto1_FREE_PORT,Moto1_FREE_PIN},
	{Moto2_FREE_PORT,Moto2_FREE_PIN},
	{Moto3_FREE_PORT,Moto3_FREE_PIN},
        {Moto4_FREE_PORT,Moto4_FREE_PIN}
};



void BujinInit(void)
{
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
     for(i=0;i<4;i++)
   	{    
      GPIO_InitStructure.GPIO_Pin = MiduMotoFree[i].bit;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(MiduMotoFree[i].port, &GPIO_InitStructure);
      
       }
      for(i=0;i<4;i++)
   	{   
           GPIO_SetBits(MiduMotoFree[i].port,MiduMotoFree[i].bit);
        }
     
    
}


