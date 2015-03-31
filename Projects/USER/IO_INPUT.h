/*************************************************************
io_input.h
***************************************************************/
#include "stm32f0xx.h"

#define  Input0_PORT   GPIOB
#define  Input0_PIN    GPIO_Pin_11

#define  Input1_PORT   GPIOB
#define  Input1_PIN    GPIO_Pin_10

#define  Input2_PORT   GPIOB
#define  Input2_PIN    GPIO_Pin_2

#define  Input3_PORT   GPIOB
#define  Input3_PIN    GPIO_Pin_1

#define  Input4_PORT   GPIOB
#define  Input4_PIN    GPIO_Pin_0


#define  Input5_PORT   GPIOA
#define  Input5_PIN    GPIO_Pin_7

#define  Input6_PORT   GPIOA
#define  Input6_PIN    GPIO_Pin_6

#define  Input7_PORT   GPIOA
#define  Input7_PIN    GPIO_Pin_5

#define  Input8_PORT   GPIOA
#define  Input8_PIN    GPIO_Pin_0

#define  Input9_PORT   GPIOB
#define  Input9_PIN    GPIO_Pin_9

#define  Input10_PORT   GPIOB
#define  Input10_PIN    GPIO_Pin_8

#define  Input11_PORT   GPIOB
#define  Input11_PIN    GPIO_Pin_7

#define  Input12_PORT   GPIOB
#define  Input12_PIN    GPIO_Pin_6

#define  Input13_PORT   GPIOB
#define  Input13_PIN    GPIO_Pin_5

#define  Input14_PORT   GPIOB
#define  Input14_PIN    GPIO_Pin_4

#define  Input15_PORT   GPIOB
#define  Input15_PIN    GPIO_Pin_3

#define  Input16_PORT   GPIOA
#define  Input16_PIN    GPIO_Pin_15


#define Max_Input_Single 	17

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;


void Io_input_Init(void);

