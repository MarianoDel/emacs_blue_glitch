//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.H #################################
//---------------------------------------------

#ifndef _HARD_H_
#define _HARD_H_


//----------- Defines For Configuration -------------

//----- Board Configuration -------------------//
//--- Hardware ------------------//
#define HARDWARE_VERSION_1_0        //placa Arduino Blue Pill

//--- Software ------------------//
// #define SOFTWARE_VERSION_1_2		
// #define SOFTWARE_VERSION_1_1	     //habla contra pc o rpi con nuevo protocolo camilla
#define SOFTWARE_VERSION_1_0        //habla contra rpi con programa magneto y traduce a micros potencia

//-------- Type of Program (depending on software version) ----------------
// #define INVERTER_SQUARE_MODE
// #define INVERTER_QUASI_SINE_WAVE
#define INVERTER_PURE_SINUSOIDAL

#define USE_FREQ_18KHZ
// #define USE_FREQ_9KHZ
// #define USE_FREQ_4_5KHZ

#ifdef INVERTER_SQUARE_MODE
// #define USE_CHANNELS_WITH_PRELOAD
#endif
//-------- Type of Program and Features ----------------

//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------

//-------- Hysteresis Conf ------------------------

//-------- PWM Conf ------------------------

//-------- Oscillator and Crystal selection (Freq in startup_clocksh) ---
#define HSI_INTERNAL_RC
// #define HSE_CRYSTAL_OSC


//-------- End Of Defines For Configuration ------




//--- Hardware & Software Messages ------------------//
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0\r\n"
#endif
#ifdef HARDWARE_VERSION_2_1
#define HARD "Hardware Version: 2.1\r\n"
#endif
#ifdef SOFTWARE_VERSION_2_2
#define SOFT "Software Version: 2.2\r\n"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\r\n"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\r\n"
#endif
//--- Type of Program Announcement ----------------
#ifdef HARDWARE_TESTS
#define FEATURES "Programa de Testeo\n LED\n Usart1\n"
#endif
#ifdef TIM1_AND_TIM3
#define FEATURES "Tim1 linked to Tim3\n"
#endif

//--- End of Hardware & Software Messages ------------------//



//--- Exported constants ---//
typedef enum {
	resp_ok = 0,
	resp_not_own,
	resp_error

} resp_t;

enum resultados
{
	NO_INIT = 0,
	TRABAJANDO,
	FIN_OK,
	FIN_ERROR,
	FIN_TIMEOUT
};

enum bool
{
	FALSE = 0,
	TRUE = !FALSE
};


#define TT_ON    9000
#define TT_DEAD_TIME    1000
#define TT_THIRD    3266
#define TT_SINE_POINT    200

#if (defined INVERTER_SQUARE_MODE) || (defined INVERTER_PURE_SINUSOIDAL)
typedef enum {
    ON_LEFT = 0,
    WAIT_DEAD_TIME_LEFT,
    ON_RIGHT,
    WAIT_DEAD_TIME_RIGHT,
    JUMPER_PROTECTED

} pin_state_t;
#endif

#ifdef INVERTER_QUASI_SINE_WAVE
typedef enum {
    ON_LEFT_RISING = 0,
    ON_LEFT_FULL,
    ON_LEFT_FALLING,
    WAIT_DEAD_TIME_LEFT,
    ON_RIGHT_RISING,
    ON_RIGHT_FULL,
    ON_RIGHT_FALLING,    
    WAIT_DEAD_TIME_RIGHT,
    JUMPER_PROTECTED

} pin_state_t;
#endif



//--- Configuracion de Pines ---//
#ifdef HARDWARE_VERSION_1_0

//--- Port A ---//

//PA5 input floating
#define SENSE_MOSFET_A ((GPIOA->IDR & 0x0020) != 0)

//PA6 Alternative TIM3_CH1

//PA8 Alternative TIM1_CH1

//PA9, PA10 Alternative Usart 1

//PA15 input pull-up
#define JUMPER_PROT ((GPIOA->IDR & 0x8000) != 0)

//--- Port B ---//
#define SENSE_MOSFET_B ((GPIOB->IDR & 0x8000) != 0)

//--- Port C ---//
//PC13
#define LED ((GPIOC->ODR & 0x2000) == 0)
#define LED_OFF GPIOC->BSRR = 0x00002000
#define LED_ON GPIOC->BSRR = 0x20000000

//old declarations & macros
//--- PB13 ---//
//Alternative TIM1_CH1N

//--- PB14 ---//
//Alternative TIM1_CH2N

//--- PB15 ---//
#define OUT1 ((GPIOB->ODR & 0x8000) == 0)
#define OUT1_OFF GPIOB->BSRR = 0x00008000
#define OUT1_ON GPIOB->BSRR = 0x80000000

//--- PC6 ---//
#define OUT2 ((GPIOC->ODR & 0x0040) != 0)
#define OUT2_ON GPIOC->BSRR = 0x00000040
#define OUT2_OFF GPIOC->BSRR = 0x00400000

//--- PC7 ---//
#define OUT3 ((GPIOC->ODR & 0x0080) != 0)
#define OUT3_ON GPIOC->BSRR = 0x00000080
#define OUT3_OFF GPIOC->BSRR = 0x00800000

//--- PC8 ---//
#define IN2 ((GPIOC->IDR & 0x0100) != 0)

//--- PC9 ---//
#define IN1 ((GPIOC->IDR & 0x0200) != 0)

//--- PA8 ---//
#define IN0 ((GPIOA->IDR & 0x0100) != 0)

//PA9
//PA10    Usart1

//PA11, PA12, PA13, PA14, PA15    NC

//PC10
//PC11    Uart4

//PC12
//PD2    Uart5

//PB3, PB4    NC

//--- PB5 ---//
#define SW_RX_TX ((GPIOB->ODR & 0x0020) != 0)
#define SW_RX_TX_ON GPIOB->BSRR = 0x00000020
#define SW_RX_TX_OFF GPIOB->BSRR = 0x00200000

//PB6, PB7, PB8, PB9    NC

#endif //HARDWARE_VERSION_1_0


// #ifdef HARDWARE_VERSION_2_1
// //--- PC13 ---//
// #define BUZZER ((GPIOC->ODR & 0x2000) != 0)
// #define BUZZER_ON GPIOC->BSRR = 0x00002000
// #define BUZZER_OFF GPIOC->BSRR = 0x20000000

// //--- PB11 prueba salida por USART3_RX ---//
// #define RX_PIN ((GPIOB->ODR & 0x0800) != 0)
// #define RX_PIN_ON GPIOB->BSRR = 0x00000800
// #define RX_PIN_OFF GPIOB->BSRR = 0x08000000

// //--- PB9 ---//
// #define L_ZONA ((GPIOB->ODR & 0x0200) != 0)
// #define L_ZONA_ON GPIOB->BSRR = 0x00000200
// #define L_ZONA_OFF GPIOB->BSRR = 0x02000000

// //--- PB8 ---//
// #define L_ALARMA ((GPIOB->ODR & 0x0100) != 0)
// #define L_ALARMA_ON GPIOB->BSRR = 0x00000100
// #define L_ALARMA_OFF GPIOB->BSRR = 0x01000000

// //--- PB7 ---//
// #define L_SERV ((GPIOB->ODR & 0x0080) != 0)
// #define L_SERV_ON GPIOB->BSRR = 0x00000080
// #define L_SERV_OFF GPIOB->BSRR = 0x00800000

// //--- PB6 ---//
// #define L_NETLIGHT ((GPIOB->ODR & 0x0040) != 0)
// #define L_NETLIGHT_ON GPIOB->BSRR = 0x00000040
// #define L_NETLIGHT_OFF GPIOB->BSRR = 0x00400000

// //--- PB5 ---//
// #define L_WIFI ((GPIOB->ODR & 0x0020) != 0)
// #define L_WIFI_ON GPIOB->BSRR = 0x00000020
// #define L_WIFI_OFF GPIOB->BSRR = 0x00200000

// //--- PB0 ---//
// #define ENA_CH1 ((GPIOB->ODR & 0x0001) != 0)
// #define ENA_CH1_ON GPIOB->BSRR = 0x00000001
// #define ENA_CH1_OFF GPIOB->BSRR = 0x00010000

// //--- PB1 ---//
// #define ENA_CH2 ((GPIOB->ODR & 0x0002) != 0)
// #define ENA_CH2_ON GPIOB->BSRR = 0x00000002
// #define ENA_CH2_OFF GPIOB->BSRR = 0x00020000

// //--- PB2 ---//
// #define ENA_CH3 ((GPIOB->ODR & 0x0004) != 0)
// #define ENA_CH3_ON GPIOB->BSRR = 0x00000004
// #define ENA_CH3_OFF GPIOB->BSRR = 0x00040000

// //--- PB13 ---//
// #define ENA_CH4 ((GPIOB->ODR & 0x2000) != 0)
// #define ENA_CH4_ON GPIOB->BSRR = 0x00002000
// #define ENA_CH4_OFF GPIOB->BSRR = 0x20000000

// #endif    //HARDWARE_VERSION_2_1

//--- RCC clkEnable ---//



#define RCC_ADC1_CLK (RCC->APB2ENR & 0x00000200)
#define RCC_ADC1_CLKEN RCC->APB2ENR |= 0x00000200
#define RCC_ADC1_CLKDIS RCC->APB2ENR &= ~0x00000200

#define RCC_ADC2_CLK (RCC->APB2ENR & 0x00000400)
#define RCC_ADC2_CLKEN RCC->APB2ENR |= 0x00000400
#define RCC_ADC2_CLKDIS RCC->APB2ENR &= ~0x00000400

#define RCC_ADC3_CLK (RCC->APB2ENR & 0x00008000)
#define RCC_ADC3_CLKEN RCC->APB2ENR |= 0x00008000
#define RCC_ADC3_CLKDIS RCC->APB2ENR &= ~0x00008000


//--- Exported Module Functions ----


#endif

//--- end of file ---//
