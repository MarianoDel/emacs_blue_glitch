//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIMER.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "stm32f10x.h"
#include "hard.h"


/* Externals ------------------------------------------------------------------*/
extern volatile unsigned short wait_ms_var;



/* Globals ------------------------------------------------------------------*/


/* Module Functions -----------------------------------------------------------*/
void Wait_ms (unsigned short a)
{
    if (a > 1)
        wait_ms_var = a - 1;
    else
        wait_ms_var = a;

    while (wait_ms_var);
}

inline void UpdateTIMSync (unsigned short a)
{
    //primero cargo TIM1
    TIM1->CCR1 = a;
    TIM3->ARR = DUTY_50_PERCENT + a;    //TIM3->CCR1 es el delay entre timers
                                        //lo cargo en el timer init
}

inline void UpdateTIM_MosfetA (unsigned short a)
{
    TIM3->ARR = DUTY_50_PERCENT + a;    
}

inline void UpdateTIM_MosfetB (unsigned short a)
{
    TIM1->CCR1 = a;
}

inline void EnablePreload_MosfetA (void)
{
    // TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM3->CR1 |= TIM_CR1_ARPE;
}

inline void DisablePreload_MosfetA (void)
{
    // TIM3->CCMR1 &= ~TIM_CCMR1_OC1PE;
    TIM3->CR1 &= ~TIM_CR1_ARPE;    
}

inline void EnablePreload_MosfetB (void)
{
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
}

inline void DisablePreload_MosfetB (void)
{
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1PE;
}

void Update_TIM1_CH1 (unsigned short a)
{
    TIM1->CCR1 = a;
}

void Update_TIM1_CH2 (unsigned short a)
{
    TIM1->CCR2 = a;
}

void Update_TIM3_CH1 (unsigned short a)
{
    TIM3->CCR1 = a;
}

void Update_TIM3_CH2 (unsigned short a)
{
    TIM3->CCR2 = a;
}

void Update_TIM3_CH3 (unsigned short a)
{
    TIM3->CCR3 = a;
}

void Update_TIM3_CH4 (unsigned short a)
{
    TIM3->CCR4 = a;
}

//-------------------------------------------//
// @brief  TIM configure.
// @param  None
// @retval None
//------------------------------------------//
void TIM_1_Init (void)
{
    if (!RCC_TIM1_CLK)
        RCC_TIM1_CLKEN;

    //Configuracion del timer.
    TIM1->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    // TIM1->CR1 = 0x00;        //clk int / 1;
    // TIM1->CR1 |= TIM_CR1_ARPE;        //clk int / 1, Preload;
    // TIM1->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    TIM1->CR2 = 0x00;
    //TIM1->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
    TIM1->SMCR = 0x0000;

    TIM1->CCMR1 = 0x0060;            //CH1 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)
#ifdef USE_CHANNELS_WITH_PRELOAD
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;
#endif
    TIM1->CCMR2 = 0x0000;
    // TIM1->CCER |= TIM_CCER_CC1NE | TIM_CCER_CC1NP | TIM_CCER_CC2NE | TIM_CCER_CC2NP;
    TIM1->CCER |= TIM_CCER_CC1E;
        
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->ARR = DUTY_100_PERCENT;    //cada tick 20.83ns con PSC = 0

    TIM1->CNT = 0;
// #if defined USE_FREQ_18KHZ
//     TIM1->PSC = 3;
// #elif defined USE_FREQ_9KHZ
//     TIM1->PSC = 7;
// #elif defined USE_FREQ_4_5KHZ
//     TIM1->PSC = 15;
// #else
// #error "Select working frequency on hard.h"
// #endif

    // Enable timer ver UDIS
    //TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CR1 |= TIM_CR1_CEN;

    TIM1->CCR1 = 0;
}

void TIM_3_Init (void)
{
    if (!RCC_TIM3_CLK)
        RCC_TIM3_CLKEN;

    //Configuracion del timer.
    TIM3->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    //TIM3->CR1 = 0x0000;        //clk int / 1; upcounting;
    //TIM3->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    TIM3->CR2 = 0x0000;
    //TIM3->SMCR |= TIM_SMCR_SMS_2 |TIM_SMCR_SMS_1 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0;    //reset mode
    //TIM3->SMCR |= TIM_SMCR_SMS_2;    //reset mode link timer1    OJO no anda
    TIM3->SMCR |= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1;    //trigger mode link timer1
    //TIM3->SMCR = 0x0000;    //
    //TIM3->CCMR1 = 0x6000;            //CH2 output PWM mode 1
    //  TIM3->CCMR1 = 0x0060;            //CH1 output PWM mode 1
    TIM3->CCMR1 = 0x0070;            //CH1 output PWM mode 2 (channel inactive TIM3->CNT < TIM3->CCR1)
    TIM3->CCMR2 = 0x0000;            //
    //  TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC1P;    //CH1 enable on pin active high
    TIM3->CCER |= TIM_CCER_CC1E;    //CH1 enable on pin active high
    //TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC2P;    //CH2 enable on pin active high
    TIM3->ARR = DUTY_50_PERCENT;
    TIM3->CNT = 0;
    TIM3->PSC = 0;
    //TIM3->EGR = TIM_EGR_UG;    //generate event

    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;

    //TIM3->CCR2 = 512;        //delay = TIM3->CCRx = 512 - TIM1->CCR2
    TIM3->CCR1 = (DUTY_50_PERCENT + 1);        //delay = TIM3->CCRx = 512 - TIM1->CCR2
}

void TIM3_IRQHandler (void)	//1 ms
{
    //Code Handler
    
    //bajar flag
    if (TIM3->SR & 0x01)	//bajo el flag
        TIM3->SR = 0x00;
}


void TIM_4_Init(void)
{
//    Counter Register (TIMx_CNT)
//    Prescaler Register (TIMx_PSC)
//    Auto-Reload Register (TIMx_ARR)
//    The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)
//
//    Quiero una interrupcion por ms CK_INT es 72MHz

    //---- Clk ----//
    if (!RCC_TIM4_CLK)
        RCC_TIM4_CLKEN;

    //--- Config ----//
    TIM4->ARR = 0xFFFF;
    TIM4->CNT = 0;
    TIM4->PSC = 71;
    TIM4->EGR = TIM_EGR_UG; //update registers

    // Enable timer ver UDIS
    TIM4->CR1 |= TIM_CR1_CEN;
}


#ifdef STM32F10X_HD
void TIM7_IRQHandler (void)	//1mS
{
    if (TIM7->SR & 0x01)
        TIM7->SR = 0x00;    //bajar flag

    if (LED2)
        LED2_OFF;
    else
        LED2_ON;
    
}


// void TIM6_IRQHandler (void)	//100mS
// {
// 	UART_Tim6 ();

// 	//bajar flag
// 	if (TIM6->SR & 0x01)	//bajo el flag
// 		TIM6->SR = 0x00;
// }


//inicializo el TIM7 para interrupciones
void TIM7_Init(void)
{
//    Counter Register (TIMx_CNT)
//    Prescaler Register (TIMx_PSC)
//    Auto-Reload Register (TIMx_ARR)
//    The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)
//
//    Quiero una interrupcion por ms CK_INT es 72MHz

    //---- Clk ----//
    if (!RCC_TIM7_CLK)
        RCC_TIM7_CLK_ON;

    //--- Config ----//
    TIM7->ARR = 1000;
    //TIM7->ARR = 100;
    TIM7->CNT = 0;
    TIM7->PSC = 71;
    TIM7->EGR = TIM_EGR_UG; //update registers

    // Enable timer ver UDIS
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;

    //Habilito NVIC
    //Interrupcion timer7.
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_SetPriority(TIM7_IRQn, 10);
}

// void TIM6_Init(void)
// {
// 	NVIC_InitTypeDef NVIC_InitStructure;
// //    		Counter Register (TIMx_CNT)
// //    		Prescaler Register (TIMx_PSC)
// //    		Auto-Reload Register (TIMx_ARR)
// //			The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)

// //			Quiero una interrupcion por ms CK_INT es 72MHz

// 	//---- Clk ----//
// 	if (!(RCC->APB1ENR & 0x00000010))
// 		RCC->APB1ENR |= 0x00000010;

// 	//--- Config ----//
// 	TIM6->ARR = 10000; //100mS.
// 	TIM6->CNT = 0;
// 	TIM6->PSC = 719;
// 	TIM6->EGR = TIM_EGR_UG;

// 	// Enable timer ver UDIS
// 	TIM6->DIER |= TIM_DIER_UIE;
// 	TIM6->CR1 |= TIM_CR1_CEN;

// 	//Habilito NVIC
// 	//Interrupcion timer6.
// 	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
// 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 11;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
// }
#endif

//--- end of file ---//
