//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GPIO.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "gpio.h"
#include "hard.h"


/* Externals ------------------------------------------------------------------*/


/* Globals ------------------------------------------------------------------*/


/* Module Functions -----------------------------------------------------------*/


//--- Tamper config ---//
void Tamper_Config(void)
{
	unsigned long temp;

	//--- GPIOB ---//
	//--- Clock ---//
	if (!(RCC->APB2ENR & 0x00000008))
		RCC->APB2ENR |= 0x00000008;

	//--- Config pines ---//
	temp = GPIOB->CRH;
	temp &= 0xFF0FFFFF;
	temp |= 0x00800000;
	GPIOB->CRH = temp;
	GPIOB->BSRR = 0x00002000;
}





//------- GPIO REGISTERS ----------//
//
//	GPIOx->CRL	pin 7 - 0
//	3  2  1  0
//	CNF0  MODE0
//	GPIOx->CRH	pin 15 - 8
//
//      En hexa me queda cada nibble es igual a la configuracion de ese pin
//      nibble0 configura completo pin0
//
//	MODEx 00 Input (reset)
//	MODEx 01 Output 10MHz
//	MODEx 10 Output 2MHz
//	MODEx 11 Output 50MHz
//
//	Input Mode
//      las funciones alternativas de los pines estan directamente conectadas al
//      periferico en el modo input
//      CNFx 00 Analog
//      CNFx 01 Floating (reset)
//      CNFx 10 Input (pull up / dwn)
//      CNFx 11 Reserved
//
//	Output Mode
//      CNFx 00 Push Pull
//      CNFx 01 Open Drain
//      CNFx 10 Alternate func Push Pull
//      CNFx 11 Alternate func Open Drain
//
//      Pull-Up Pull-Dwn si el pin es Input el registro ODR coloca pull-up (1) o pull-dwn (0)
//      GPIOx->ODR 0xNNNN, 1 bit por pin
//

//-- GPIO Configuration --------------------
void GpioInit (void)
{
    unsigned long temp;

    //--- GPIO Clocks ---//
    if (!RCC_GPIOA_CLK)
        RCC_GPIOA_CLKEN;

    if (!RCC_GPIOB_CLK)
        RCC_GPIOB_CLKEN;

    if (!RCC_GPIOC_CLK)
        RCC_GPIOC_CLKEN;

    if (!RCC_GPIOD_CLK)
        RCC_GPIOD_CLKEN;

    //--- GPIOA Low Side ------------------//
    temp = GPIOA->CRL;    //PA0 Input pull-up (Interrupt) PA2-PA3 Alternative (Usart2); 
    temp &= 0xF00F00F0;    //PA5 input floating; PA6 Alternative (TIM3_CH1)
    temp |= 0x0A408A08;    
    GPIOA->CRL = temp;

    //--- GPIOA High Side ------------------//
    temp = GPIOA->CRH;    //PA8 Alterantive (TIM1_CH1); PA9-PA10 Alternative (Usart1);
    temp &= 0x0FFFF000;    //PA15 input w/pu
    temp |= 0x800008AA;
    GPIOA->CRH = temp;

    //--- GPIOA Pull-Up Pull-Dwn ------------------//
    temp = GPIOA->ODR;    //PA0 pull-up PA3 pull-up
    temp &= 0x7FF6;    //PA15 pull-up
    temp |= 0x8009;
    GPIOA->ODR = temp;
    
    //--- GPIOB Low Side -------------------//
    temp = GPIOB->CRL;    //PB5 output
    temp &= 0xFF0FFFFF;
    temp |= 0x00200000;
    GPIOB->CRL = temp;

    //--- GPIOB High Side -------------------//
    temp = GPIOB->CRH;    //PB10-PB11 Alternative (Usart3); PB13 - PB14 Alternative (TIM1)
    temp &= 0x000F00FF;
    temp |= 0x4AA08A00;
    GPIOB->CRH = temp;    
    
    //--- GPIOC Low Side -------------------//
    temp = GPIOC->CRL;    //PC0-PC1 output; PC6-PC7 output
    temp &= 0x00FFFF00;
    temp |= 0x22000022;
    GPIOC->CRL = temp;

    //--- GPIOC High Side -------------------//    
    temp = GPIOC->CRH;    
    // temp &= 0xFFF00000;    //PC8-PC9 Input; PC10-PC11 Alternative (Uart4); PC12 Alterantive (Uart5)
    // temp |= 0x000A8A88;
    temp &= 0xFF0FFFFF;    //PC13 output
    temp |= 0x00200000;
    GPIOC->CRH = temp;

    //--- GPIOD Low Side -------------------//
    temp = GPIOD->CRL;    //PD2 Alterantive (Uart5)
    temp &= 0xFFFFF0FF;    
    temp |= 0x00000A00;
    GPIOD->CRL = temp;

#ifdef USE_EXTERNAL_INTS
    //Interrupt en PA4 y PA5
    if (!RCC_AFIO_CLK)
        RCC_AFIO_CLKEN;

    //Select Port A & Pin0 for external interrupt
    temp = AFIO->EXTICR[0];
    temp &= ~AFIO_EXTICR1_EXTI0;
    temp |= AFIO_EXTICR1_EXTI0_PA;
    AFIO->EXTICR[0] = (unsigned short) temp;
    
    // EXTI->IMR |= 0x00000001; 			//Corresponding mask bit for interrupts EXTI0
    EXTI->EMR |= 0x00000000; 			//Corresponding mask bit for events
    EXTI->RTSR |= 0x00000001; 			//Interrupt line on rising edge
    EXTI->FTSR |= 0x00000000; 			//Interrupt line on falling edge

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_SetPriority(EXTI0_IRQn, 2);

#endif
}

#ifdef USE_EXTERNAL_INTS
inline void EXTIOff (void)
{
    EXTI->IMR &= ~0x00000001;
}

inline void EXTIOn (void)
{
    EXTI->IMR |= 0x00000001;
}
#endif

//--- end of file ---//
