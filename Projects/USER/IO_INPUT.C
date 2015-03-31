/*******************************************************************************
IO_INPUT.C
********************************************************************************/
#include "io_input.h"


const GPIOGROUP  Input_Single[Max_Input_Single]={
 	{Input0_PORT,Input0_PIN},
	{Input1_PORT,Input1_PIN},
	{Input2_PORT,Input2_PIN},
	{Input3_PORT,Input3_PIN},
	{Input4_PORT,Input4_PIN},
	{Input5_PORT,Input5_PIN},
	{Input6_PORT,Input6_PIN},
	{Input7_PORT,Input7_PIN},
	{Input8_PORT,Input8_PIN},
	{Input9_PORT,Input9_PIN},
	{Input10_PORT,Input10_PIN},
	{Input11_PORT,Input11_PIN},
	{Input12_PORT,Input12_PIN},
	{Input13_PORT,Input13_PIN},
	{Input14_PORT,Input14_PIN},
	{Input15_PORT,Input15_PIN},
	{Input16_PORT,Input16_PIN}						
};
//IO inint 
void Io_input_Init(void)
{
	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
	for( i = 0 ; i < Max_Input_Single ; i ++ )
	{    
		GPIO_InitStructure.GPIO_Pin = Input_Single[i].bit;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		
		GPIO_Init(Input_Single[i].port, &GPIO_InitStructure);
	}	
}
