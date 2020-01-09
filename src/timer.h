//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIMER.H ################################
//---------------------------------------------

#ifndef _TIMER_H_
#define _TIMER_H_

//-- Exported Macros ------------------------------
#define DUTY_10_PERCENT		100
#define DUTY_50_PERCENT		500
#define DUTY_100_PERCENT        1000
#define DUTY_FOR_DMAX           450

#define DUTY_ALWAYS    (DUTY_100_PERCENT + 1)
#define DUTY_NONE    0

//-- Exported Hardware Macros ---------------------
#define ENABLE_TIM1			TIM1->CR1 |= TIM_CR1_CEN;
#define DISABLE_TIM1			TIM1->CR1 &= ~TIM_CR1_CEN;

#define ENABLE_TIM3			TIM3->CR1 |= TIM_CR1_CEN;
#define DISABLE_TIM3			TIM3->CR1 &= ~TIM_CR1_CEN;

#ifdef STM32F10X_HD
#define EnableTimer6 TIM6->CR1 |= TIM_CR1_CEN
#define DisableTimer6 TIM6->CR1 &= ~TIM_CR1_CEN

#define EnableTimer7 TIM7->CR1 |= TIM_CR1_CEN
#define DisableTimer7 TIM7->CR1 &= ~TIM_CR1_CEN
#endif

#define RCC_TIM1_CLK 		(RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLKEN     	RCC->APB2ENR |= 0x00000800
#define RCC_TIM1_CLKDIS 	RCC->APB2ENR &= ~0x00000800

#define RCC_TIM2_CLK            (RCC->APB1ENR & 0x00000001)
#define RCC_TIM2_CLKEN          RCC->APB1ENR |= 0x00000001
#define RCC_TIM2_CLKDIS         RCC->APB1ENR &= ~0x00000001

#define RCC_TIM3_CLK 		(RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLKEN     	RCC->APB1ENR |= 0x00000002
#define RCC_TIM3_CLKDIS    	RCC->APB1ENR &= ~0x00000002

#define RCC_TIM4_CLK            (RCC->APB1ENR & 0x00000004)
#define RCC_TIM4_CLKEN          RCC->APB1ENR |= 0x00000004
#define RCC_TIM4_CLKDIS         RCC->APB1ENR &= ~0x00000004

#ifdef STM32F10X_HD
#define RCC_TIM5_CLK            (RCC->APB1ENR & 0x00000008)
#define RCC_TIM5_CLKEN          RCC->APB1ENR |= 0x00000008
#define RCC_TIM5_CLKDIS         RCC->APB1ENR &= ~0x00000008

#define RCC_TIM6_CLK            (RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLKEN          RCC->APB1ENR |= 0x00000010;
#define RCC_TIM6_CLKDIS         RCC->APB1ENR &= ~0x00000010;

#define RCC_TIM7_CLK            (RCC->APB1ENR & 0x00000020)
#define RCC_TIM7_CLKEN          RCC->APB1ENR |= 0x00000020;
#define RCC_TIM7_CLKDIS         RCC->APB1ENR &= ~0x00000020;
#endif

#define PIN_LEFT_ON    Update_TIM1_CH1(DUTY_ALWAYS)
#define PIN_LEFT_OFF    Update_TIM1_CH1(DUTY_NONE)
#define PIN_LEFT_50    Update_TIM1_CH1(DUTY_50_PERCENT)    
#define PIN_RIGHT_ON    Update_TIM1_CH2(DUTY_ALWAYS)
#define PIN_RIGHT_50    Update_TIM1_CH2(DUTY_50_PERCENT)
#define PIN_RIGHT_OFF    Update_TIM1_CH2(DUTY_NONE)

#define PIN_LEFT_PWM(X)    Update_TIM1_CH1(X)
#define PIN_RIGHT_PWM(X)    Update_TIM1_CH2(X)

//--- Exported Functions ------------------------------
void Wait_ms (unsigned short);

// void TIM2_Init(void);
void TIM_1_Init (void);
void TIM_3_Init (void);

void UpdateTIMSync (unsigned short);
void UpdateTIM_MosfetA (unsigned short);
void UpdateTIM_MosfetB (unsigned short);
void EnablePreload_MosfetA (void);
void DisablePreload_MosfetA (void);
void EnablePreload_MosfetB (void);
void DisablePreload_MosfetB (void);

void Update_TIM1_CH1 (unsigned short);
void Update_TIM1_CH2 (unsigned short);

void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);

void TIM_4_Init (void);

#ifdef STM32F10X_HD
void TIM6_Init(void);
void TIM6_IRQHandler (void);
void TIM7_Init(void);
void TIM7_IRQHandler (void);
#endif

#endif /* _TIMER_H_ */
