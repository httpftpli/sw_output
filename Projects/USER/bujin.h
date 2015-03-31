#include "stm32f0xx.h"


#define  Moto1_FREE_PORT   GPIOA
#define  Moto1_FREE_PIN    GPIO_Pin_3

#define  Moto2_FREE_PORT   GPIOB
#define  Moto2_FREE_PIN    GPIO_Pin_10

#define  Moto3_FREE_PORT   GPIOB
#define  Moto3_FREE_PIN    GPIO_Pin_15

#define  Moto4_FREE_PORT   GPIOB
#define  Moto4_FREE_PIN    GPIO_Pin_9 

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;

void BujinInit(void);

