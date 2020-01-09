//----------------------------------------------------------------------
// #### PROYECTO PARA PLACA ARDUINO STM32 - Arduino Blue Pill Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### MAIN.C ############################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "hard.h"

#include "timer.h"
#include "gpio.h"
#include "usart.h"

#include <stdio.h>



/* Externals ------------------------------------------------------------------*/
//--- Externals para avisar data ready en usart
volatile unsigned char usart1_have_data;
volatile unsigned char usart2_have_data;
volatile unsigned char usart3_have_data;
#ifdef STM32F10X_HD
volatile unsigned char usart4_have_data;
volatile unsigned char usart5_have_data;
#endif

unsigned short comms_messages = 0;
char buffSendErr[64];

//--- Externals para enviar keepalive por UART
#define TIME_RUN_DEF 250
volatile unsigned short timeRun = TIME_RUN_DEF;

//--- Externals para muestreos de corriente con el ADC
volatile unsigned char flagMuestreo = 0;
volatile unsigned char take_current_samples = 0;

//--- Externals para armar seniales y comprobar el TIM5 en el inicio del programa
volatile unsigned int session_warming_up_channel_1_stage_time = 0;

//--- Externals para el BUZZER
unsigned short buzzer_timeout = 0;

//--- Externals de los timers
volatile unsigned short wait_ms_var = 0;
volatile unsigned short comms_timeout = 0;
volatile unsigned short timer_standby = 0;


/* Globals ------------------------------------------------------------------*/
#define SIZEOF_SIGNAL    50

unsigned short mem_signal [SIZEOF_SIGNAL] = {62,125,187,248,309,368,425,481,535,587,
                                             637,684,728,770,809,844,876,904,929,951,
                                             968,982,992,998,1000,998,992,982,968,951,
                                             929,904,876,844,809,770,728,684,637,587,
                                             535,481,425,368,309,248,187,125,62,0};


unsigned short * p_signal;

//--- Module Functions Declarations ----------
void TimingDelay_Decrement(void);
extern void EXTI0_IRQHandler (void);

//--- Module Function Definitions ----------

int main (void)
{
    unsigned char i = 0;
    unsigned long ii = 0;
#if (defined INVERTER_SQUARE_MODE) || (defined INVERTER_PURE_SINUSOIDAL)
    pin_state_t pin_state = ON_LEFT;
#endif

#ifdef INVERTER_QUASI_SINE_WAVE
    pin_state_t pin_state = ON_LEFT_RISING;
#endif

    //Configuracion systick    
    if (SysTick_Config(72000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

    // Configuracion led. & Enabled Channels
    GpioInit();

    //enciendo usart1
    Usart1Config();

    // //enciendo usart2 para comunicacion con micros
    // Usart2Config();
    

    //-- Welcome Messages --------------------
#ifdef HARD
    Usart1Send(HARD);
    Wait_ms(100);    
#else
#error	"No Hardware defined in hard.h file"
#endif

#ifdef SOFT
    Usart1Send(SOFT);
    Wait_ms(100);    
#else
#error	"No Soft Version defined in hard.h file"
#endif
#ifdef FEATURES
    Usart1Send((const char *) FEATURES);
    Wait_ms(100);
#endif

    TIM_1_Init();
    TIM_4_Init();
    
#ifdef INVERTER_SQUARE_MODE
    PIN_LEFT_OFF;
    PIN_RIGHT_OFF;
    while (1)
    {
        if (JUMPER_PROT)
        {
            PIN_LEFT_OFF;
            PIN_RIGHT_OFF;
            pin_state = JUMPER_PROTECTED;
            timer_standby = 1000;
        }
        
        switch (pin_state)
        {
        case ON_LEFT:
            if (TIM4->CNT > TT_ON)
            {
                TIM4->CNT = 0;
                PIN_LEFT_OFF;
                pin_state = WAIT_DEAD_TIME_LEFT;
            }
            break;

        case WAIT_DEAD_TIME_LEFT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                PIN_RIGHT_ON;
                pin_state = ON_RIGHT;
            }
            break;

        case ON_RIGHT:
            if (TIM4->CNT > TT_ON)
            {
                TIM4->CNT = 0;
                PIN_RIGHT_OFF;
                pin_state = WAIT_DEAD_TIME_RIGHT;
            }
            break;

        case WAIT_DEAD_TIME_RIGHT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                PIN_LEFT_ON;
                pin_state = ON_LEFT;
            }
            break;

        case JUMPER_PROTECTED:
            if (!timer_standby)
            {
                if (!JUMPER_PROT)
                {
                    TIM4->CNT = 0;
                    PIN_LEFT_ON;
                    pin_state = ON_LEFT;
                }
            }
            break;
            
        default:
            TIM4->CNT = 0;
            PIN_LEFT_ON;
            pin_state = ON_LEFT;
            break;
        }
    }
#endif    //INVERTER_SQUARE_MODE

#ifdef INVERTER_QUASI_SINE_WAVE
    PIN_LEFT_OFF;
    PIN_RIGHT_OFF;    
    while (1)
    {
        if (JUMPER_PROT)
        {
            PIN_LEFT_OFF;
            PIN_RIGHT_OFF;
            pin_state = JUMPER_PROTECTED;
            timer_standby = 1000;
        }

        switch (pin_state)
        {
        case ON_LEFT_RISING:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_LEFT_ON;
                pin_state = ON_LEFT_FULL;
            }
            break;

        case ON_LEFT_FULL:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_LEFT_50;
                pin_state = ON_LEFT_FALLING;
            }
            break;
            
        case ON_LEFT_FALLING:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_LEFT_OFF;
                pin_state = WAIT_DEAD_TIME_LEFT;
            }
            break;
            
        case WAIT_DEAD_TIME_LEFT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                PIN_RIGHT_50;
                pin_state = ON_RIGHT_RISING;
            }
            break;

        case ON_RIGHT_RISING:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_RIGHT_ON;
                pin_state = ON_RIGHT_FULL;
            }
            break;

        case ON_RIGHT_FULL:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_RIGHT_50;
                pin_state = ON_RIGHT_FALLING;
            }
            break;

        case ON_RIGHT_FALLING:
            if (TIM4->CNT > TT_THIRD)
            {
                TIM4->CNT = 0;
                PIN_RIGHT_OFF;
                pin_state = WAIT_DEAD_TIME_RIGHT;
            }
            break;
            
        case WAIT_DEAD_TIME_RIGHT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                PIN_LEFT_50;
                pin_state = ON_LEFT_RISING;
            }
            break;

        case JUMPER_PROTECTED:
            if (!timer_standby)
            {
                if (!JUMPER_PROT)
                {
                    TIM4->CNT = 0;
                    PIN_LEFT_ON;
                    pin_state = ON_LEFT_RISING;
                }
            }
            break;
            
        default:
            TIM4->CNT = 0;
            PIN_LEFT_50;
            pin_state = ON_LEFT_RISING;
            break;
        }
    }
#endif    //INVERTER_QUASI_SINE_WAVE

#ifdef INVERTER_PURE_SINUSOIDAL
    p_signal = mem_signal;
    
    PIN_LEFT_OFF;
    PIN_RIGHT_OFF;    
    while (1)
    {
        if (JUMPER_PROT)
        {
            PIN_LEFT_OFF;
            PIN_RIGHT_OFF;
            pin_state = JUMPER_PROTECTED;
            timer_standby = 1000;
        }
        
        switch (pin_state)
        {
        case ON_LEFT:
            if (TIM4->CNT >= TT_SINE_POINT)
            {
                TIM4->CNT = 0;

                if (p_signal < &mem_signal[(SIZEOF_SIGNAL - 1)])
                {                    
                    p_signal++;
                    PIN_LEFT_PWM(*p_signal);
                }
                else
                {
                    //termine senial
                    PIN_LEFT_PWM(DUTY_NONE);
                    pin_state = WAIT_DEAD_TIME_LEFT;
                }
            }
            break;
            
        case WAIT_DEAD_TIME_LEFT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                pin_state = ON_RIGHT;
                p_signal = mem_signal;
            }
            break;

        case ON_RIGHT:
            if (TIM4->CNT >= TT_SINE_POINT)
            {
                TIM4->CNT = 0;

                if (p_signal < &mem_signal[(SIZEOF_SIGNAL - 1)])
                {                    
                    p_signal++;
                    PIN_RIGHT_PWM(*p_signal);
                }
                else
                {
                    //termine senial
                    PIN_RIGHT_PWM(DUTY_NONE);
                    pin_state = WAIT_DEAD_TIME_RIGHT;
                }
            }
            break;
            
        case WAIT_DEAD_TIME_RIGHT:
            if (TIM4->CNT > TT_DEAD_TIME)
            {                
                TIM4->CNT = 0;
                pin_state = ON_LEFT;
                p_signal = mem_signal;
            }
            break;

        case JUMPER_PROTECTED:
            if (!timer_standby)
            {
                if (!JUMPER_PROT)
                {
                    TIM4->CNT = 0;
                    PIN_LEFT_ON;
                    pin_state = ON_LEFT;
                }
            }
            break;
            
        default:
            TIM4->CNT = 0;
            PIN_LEFT_OFF;
            PIN_RIGHT_OFF;
            pin_state = ON_LEFT;
            break;
        }
    }
#endif    //INVERTER_PURE_SINUSOIDAL
    
}

//--- End of Main ---//

//--- Module Functions Definitions ----------------------
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (comms_timeout)
        comms_timeout--;
    
    if (timer_standby)
        timer_standby--;

    // if (timer_filters)
    //     timer_filters--;
    
    // if (timer_led)
    //     timer_led--;

    // if (timer_led_pwm < 0xFFFF)
    //     timer_led_pwm ++;
    
}

void EXTI0_IRQHandler (void)
{
    if(EXTI->PR & 0x00000001)	//Line0
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        if (SENSE_MOSFET_A)
        {
            DisablePreload_MosfetA();
            UpdateTIM_MosfetA(0);
            EnablePreload_MosfetA();
            UpdateTIM_MosfetA(DUTY_FOR_DMAX);            
        }
        else if (SENSE_MOSFET_B)
        {
            DisablePreload_MosfetB();
            UpdateTIM_MosfetB(0);
            EnablePreload_MosfetB();
            UpdateTIM_MosfetB(DUTY_FOR_DMAX);
        }
        else
        {
            //llegue tarde
        }
        
        EXTI->PR |= 0x00000001;
    }
}

//--- end of file ---//

