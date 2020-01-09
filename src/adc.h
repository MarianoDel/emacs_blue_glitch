//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ADC.H #################################
//---------------------------------------------
#ifndef _ADC_H_
#define _ADC_H_


// Defines for Configuration ---------------------------------------------------


// Module Exported Constants ---------------------------------------------------
#define RCC_ADC1_CLK (RCC->APB2ENR & 0x00000200)
#define RCC_ADC1_CLKEN RCC->APB2ENR |= 0x00000200
#define RCC_ADC1_CLKDIS RCC->APB2ENR &= ~0x00000200

#define RCC_ADC2_CLK (RCC->APB2ENR & 0x00000400)
#define RCC_ADC2_CLKEN RCC->APB2ENR |= 0x00000400
#define RCC_ADC2_CLKDIS RCC->APB2ENR &= ~0x00000400

#define RCC_ADC3_CLK (RCC->APB2ENR & 0x00008000)
#define RCC_ADC3_CLKEN RCC->APB2ENR |= 0x00008000
#define RCC_ADC3_CLKDIS RCC->APB2ENR &= ~0x00008000

enum canalADC
{
	ADC_CHANNEL1 = 0x01,
	ADC_CHANNEL2 = 0x02,
	ADC_CHANNEL3 = 0x04,
	ADC_CHANNEL4 = 0x08,
	ADC_CHANNEL5 = 0x10,
	ADC_CHANNEL6 = 0x20
};


// Module Exported Functions ---------------------------------------------------
void  ADC1_Init(void);
unsigned char ADC1_Muestrear (unsigned char canal, unsigned short * valorMedido);
unsigned char ADC1_Scan (unsigned char canales, unsigned short * valorMedido);
void ADC_TIM7_ISR(void);


#endif    /* _ADC_H_ */

