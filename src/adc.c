
#include "adc.h"
#include "stm32f10x_adc.h"
#include "misc.h"
#include "GTK_Hard.h"


#define BUFF_ADC_DIMENSION 	8
#define BUFF_ADC_DIVISION	3

volatile unsigned short buffADC[BUFF_ADC_DIMENSION];
volatile unsigned short * pbuffADC_Write;
volatile unsigned short * pbuffADC_Read;
unsigned char ADC1_MUESTREAR_TimeOut;

//--- Current limit Externals ---//
extern volatile unsigned char flagMuestreo;

void ADC_TIM7_ISR(void)
{
	if (ADC1_MUESTREAR_TimeOut)
		ADC1_MUESTREAR_TimeOut--;
}

void ADC1_2_IRQHandler (void)
{
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC))
	{

		flagMuestreo = 1;
//		*pbuffADC_Write = ADC1->DR;
//
//		if (pbuffADC_Write != &buffADC[BUFF_ADC_DIMENSION])
//		{
//			pbuffADC_Write++;
//			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//		}

		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}

void  ADC1_Init(void)
{

	unsigned int temp;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Configuracion ADC.
	if (!RCC_GPIOA_clk)
		RCC_GPIOA_clkEnable;

	if (!RCC_GPIOC_clk)
		RCC_GPIOC_clkEnable;

	if (!RCC_ADC1_CLK)
		RCC_ADC1_CLKEN;

	//PA: 4,5,6,7
	temp = GPIOA->CRL;
	temp &= 0x0000FFFF;
	temp |= 0x00000000;
	GPIOA->CRL = temp;

	//PC: 4,5
	temp = GPIOC->CRL;
	temp &= 0xFF00FFFF;
	temp |= 0x00000000;
	GPIOC->CRL = temp;

	//clock for ADC (max 14MHz --> 72/6 = 12MHz)
	RCC_ADCCLKConfig (RCC_PCLK2_Div6);

	//define ADC config
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	// we work in continuous sampling mode
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;

	ADC_Init ( ADC1, &ADC_InitStructure);	//set config of ADC1

	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// enable ADC
	ADC_Cmd (ADC1,ENABLE);	//enable ADC1

	//	ADC calibration (optional, but recommended at power on)
	ADC_ResetCalibration(ADC1);	// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));

	// start conversion
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	ADC_Cmd (ADC1,ENABLE);	//enable ADC1

	pbuffADC_Write = &buffADC[0];
}

unsigned char ADC1_MUESTREAR_State = 0;
#define ADC1_MUESTREAR_TIMEOUT 2

enum ADC1_MUESTREAR_ESTADOS
{
	ADC1_MUESTREAR_INICIO = 0,
	ADC1_MUESTREAR_CONFIGURAR_CANAL,
	ADC1_MUESTREAR_REINICIO_BUFFER,
	ADC1_MUESTREAR_TOMAR_MUESTRA,
	ADC1_MUESTREAR_PROMEDIAR,
	ADC1_MUESTREAR_FIN
};

unsigned char ADC1_Muestrear (unsigned char canal, unsigned short * valorMedido)
{
	unsigned int suma = 0;
	unsigned char i;

	switch(ADC1_MUESTREAR_State)
	{
		case ADC1_MUESTREAR_INICIO:

			ADC1_MUESTREAR_TimeOut = ADC1_MUESTREAR_TIMEOUT;
			ADC1_MUESTREAR_State = ADC1_MUESTREAR_CONFIGURAR_CANAL;
			//LED3_ON;
			break;

		case ADC1_MUESTREAR_CONFIGURAR_CANAL:

			switch(canal)
			{
				case ADC_CHANNEL1:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_4, 1, ADC_SampleTime_28Cycles5);
					break;

				case ADC_CHANNEL2:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_5, 1,ADC_SampleTime_28Cycles5);
					break;

				case ADC_CHANNEL3:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_6, 1,ADC_SampleTime_28Cycles5);
					break;

				case ADC_CHANNEL4:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_7, 1,ADC_SampleTime_28Cycles5);
					break;

				case ADC_CHANNEL5:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_14, 1,ADC_SampleTime_28Cycles5);
					break;

				case ADC_CHANNEL6:
					ADC_RegularChannelConfig(ADC1,ADC_Channel_15, 1,ADC_SampleTime_28Cycles5);
					break;
			}

			ADC1_MUESTREAR_State = ADC1_MUESTREAR_REINICIO_BUFFER;
			break;

		case ADC1_MUESTREAR_REINICIO_BUFFER:

			pbuffADC_Write = &buffADC[0];
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			ADC1_MUESTREAR_State = ADC1_MUESTREAR_TOMAR_MUESTRA;
			break;

		case ADC1_MUESTREAR_TOMAR_MUESTRA:

			if (pbuffADC_Write == &buffADC[BUFF_ADC_DIMENSION])
			{
				suma = 0;
				for (i = 0; i < BUFF_ADC_DIMENSION; i++)
					suma += buffADC[i];
				suma = suma >> BUFF_ADC_DIVISION;
				* valorMedido = (unsigned short)(suma & 0xFFFF);
				ADC1_MUESTREAR_State = ADC1_MUESTREAR_INICIO;
				//LED3_OFF;
				return FIN_OK;
			}
			break;

		default:
			ADC1_MUESTREAR_State = ADC1_MUESTREAR_INICIO;
			break;
	}

	if (ADC1_MUESTREAR_TimeOut == 0)
		return FIN_TIMEOUT;

	return TRABAJANDO;
}

unsigned char ADC1_SCAN_State = 0;
#define ADC1_SCAN_TIMEOUT 2

enum ADC1_SCAN_ESTADOS
{
	ADC1_SCAN_INICIO = 0,
	ADC1_SCAN_CONFIGURAR_CANAL_1,
	ADC1_SCAN_TOMAR_MUESTRA_1,
	ADC1_SCAN_CONFIGURAR_CANAL_2,
	ADC1_SCAN_TOMAR_MUESTRA_2,
	ADC1_SCAN_CONFIGURAR_CANAL_3,
	ADC1_SCAN_TOMAR_MUESTRA_3,
	ADC1_SCAN_CONFIGURAR_CANAL_4,
	ADC1_SCAN_TOMAR_MUESTRA_4,
};

unsigned char ADC1_Scan (unsigned char canales, unsigned short * valorMedido)
{
	unsigned char i = 0;
	unsigned int suma;

	switch (ADC1_SCAN_State)
	{
		case ADC1_SCAN_INICIO:

			//Inicializa el vector de salida.
			for (i = 0; i<4; i++)
				*(valorMedido + i) = 0xFFFF;

			ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_1;
			break;

		case ADC1_SCAN_CONFIGURAR_CANAL_1:

			if (canales & ADC_CHANNEL1)
			{
				ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_28Cycles5);
				pbuffADC_Write = &buffADC[0];
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				ADC1_SCAN_State = ADC1_SCAN_TOMAR_MUESTRA_1;
			}
			else
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_2;
			break;

		case ADC1_SCAN_TOMAR_MUESTRA_1:


			if (pbuffADC_Write == &buffADC[BUFF_ADC_DIMENSION])
			{
				suma = 0;
				for (i = 0; i < BUFF_ADC_DIMENSION; i++)
					suma += buffADC[i];
				suma = suma >> BUFF_ADC_DIVISION;
				* valorMedido = (unsigned short)(suma & 0xFFFF);
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_2;
			}
			break;

		case ADC1_SCAN_CONFIGURAR_CANAL_2:

			if (canales & ADC_CHANNEL2)
			{
				ADC_RegularChannelConfig(ADC1,ADC_Channel_5, 1,ADC_SampleTime_28Cycles5);
				pbuffADC_Write = &buffADC[0];
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				ADC1_SCAN_State = ADC1_SCAN_TOMAR_MUESTRA_2;
			}
			else
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_3;
			break;

		case ADC1_SCAN_TOMAR_MUESTRA_2:

			if (pbuffADC_Write == &buffADC[BUFF_ADC_DIMENSION])
			{
				suma = 0;
				for (i = 0; i < BUFF_ADC_DIMENSION; i++)
					suma += buffADC[i];
				suma = suma >> BUFF_ADC_DIVISION;
				* (valorMedido + 1) = (unsigned short)(suma & 0xFFFF);
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_3;

			}
			break;

		case ADC1_SCAN_CONFIGURAR_CANAL_3:

			if (canales & ADC_CHANNEL3)
			{
				ADC_RegularChannelConfig(ADC1,ADC_Channel_6, 1,ADC_SampleTime_28Cycles5);
				pbuffADC_Write = &buffADC[0];
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				ADC1_SCAN_State = ADC1_SCAN_TOMAR_MUESTRA_3;
			}
			else
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_4;
			break;

		case ADC1_SCAN_TOMAR_MUESTRA_3:

			if (pbuffADC_Write == &buffADC[BUFF_ADC_DIMENSION])
			{
				suma = 0;
				for (i = 0; i < BUFF_ADC_DIMENSION; i++)
					suma += buffADC[i];
				suma = suma >> BUFF_ADC_DIVISION;
				* (valorMedido + 2) = (unsigned short)(suma & 0xFFFF);
				ADC1_SCAN_State = ADC1_SCAN_CONFIGURAR_CANAL_4;

			}
			break;

		case ADC1_SCAN_CONFIGURAR_CANAL_4:

			if (canales & ADC_CHANNEL4)
			{
				ADC_RegularChannelConfig(ADC1,ADC_Channel_7, 1,ADC_SampleTime_28Cycles5);
				pbuffADC_Write = &buffADC[0];
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				ADC1_SCAN_State = ADC1_SCAN_TOMAR_MUESTRA_4;
			}
			else
				ADC1_SCAN_State = ADC1_SCAN_INICIO;
			break;

		case ADC1_SCAN_TOMAR_MUESTRA_4:

			if (pbuffADC_Write == &buffADC[BUFF_ADC_DIMENSION])
			{
				suma = 0;
				for (i = 0; i < BUFF_ADC_DIMENSION; i++)
					suma += buffADC[i];
				suma = suma >> BUFF_ADC_DIVISION;
				* (valorMedido + 3) = (unsigned short)(suma & 0xFFFF);
				ADC1_SCAN_State = ADC1_SCAN_INICIO;
				return FIN_OK;
			}
			break;

		default:
			ADC1_SCAN_State = ADC1_SCAN_INICIO;
			break;
	}

	return TRABAJANDO;
}
