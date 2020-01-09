#ifndef ADC_H_
#define ADC_H_

void  ADC1_Init(void);
unsigned char ADC1_Muestrear (unsigned char canal, unsigned short * valorMedido);
unsigned char ADC1_Scan (unsigned char canales, unsigned short * valorMedido);
void ADC_TIM7_ISR(void);


enum canalADC
{
	ADC_CHANNEL1 = 0x01,
	ADC_CHANNEL2 = 0x02,
	ADC_CHANNEL3 = 0x04,
	ADC_CHANNEL4 = 0x08,
	ADC_CHANNEL5 = 0x10,
	ADC_CHANNEL6 = 0x20
};

#endif
