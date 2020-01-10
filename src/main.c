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
#include <string.h>



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
volatile unsigned short timer_led = 0;


/* Globals ------------------------------------------------------------------*/


//--- Module Functions Declarations ----------
void TimingDelay_Decrement(void);
extern void EXTI0_IRQHandler (void);

//--- Module Function Definitions ----------

int main (void)
{
    prog_state_t prog_state = PROG_RESET;
    
    // System Clock is already configured at this point

    // Set Gpios
    GpioInit ();
    
    //Configuracion systick    
    if (SysTick_Config(72000))
    {
        while (1)	/* Capture error */
        {
            if (LED)
                LED_OFF;
            else
                LED_ON;

            for (unsigned char i = 0; i < 255; i++)
            {
                asm (	"nop \n\t"
                        "nop \n\t"
                        "nop \n\t" );
            }
        }
    }

#ifdef HARD_TEST_MODE_TIMER1_OPM
    // Activate Pulses on Timer1
    TIM_1_Init();
    Update_TIM1_CH1(DUTY_50_PERCENT);
    while (1)
    {
        Wait_ms(1);
        ENABLE_TIM1;
    }
#endif
    

#ifdef HARD_TEST_MODE_USART1_TX    
    //enciendo usart1
    Usart1Config();

    while (1)
    {
        Wait_ms(1800);
        LED_ON;
        Usart1Send("Prueba Usart1\n");
        Wait_ms(200);
        LED_OFF;
    }
#endif

    
#ifdef HARD_TEST_MODE_USART1_RX
    //enciendo usart1
    Usart1Config();
    char buff_local [128] = { 0 };
    unsigned char readed = 0;

    while(1)
    {
        Wait_ms(3000);
        if (usart1_have_data)
        {
            usart1_have_data = 0;
            readed = ReadUsart1Buffer((unsigned char *)buff_local, 127);
            *(buff_local + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
            *(buff_local + readed + 1) = '\0';    //ajusto el '\0'
            Usart1Send(buff_local);
        }
    }    
#endif


#ifdef HARD_TEST_MODE_USART3_TX    
    //enciendo usart1
    Usart3Config();

    while (1)
    {
        Wait_ms(1800);
        LED_ON;
        Usart3Send("Prueba Usart3\n");
        Wait_ms(200);
        LED_OFF;
    }
#endif

    
#ifdef HARD_TEST_MODE_USART3_RX
    //enciendo usart1
    Usart3Config();
    char buff_local [128] = { 0 };
    unsigned char readed = 0;

    while(1)
    {
        Wait_ms(3000);
        if (usart3_have_data)
        {
            usart3_have_data = 0;
            readed = ReadUsart3Buffer((unsigned char *)buff_local, 127);
            *(buff_local + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
            *(buff_local + readed + 1) = '\0';    //ajusto el '\0'
            Usart3Send(buff_local);
        }
    }    
#endif
    
#ifdef GLITCHER_WITH_P0_14
    Usart1Config();
    Usart3Config();
    char buff_local_pc [128] = { 0 };
    char buff_local_bd [128] = { 0 };    
    unsigned char readed = 0;

    
    while (1)
    {
        switch(prog_state)
        {
        case PROG_RESET:
            LED_OFF;
            RESET_ON;
            Wait_ms(2);
            P0_14_ON;
            RESET_OFF;

            //reseteo autobaud
            Usart1_Autobaud();
            Wait_ms(3);

            prog_state = PROG_WAIT_AUTOBAUD;
            timer_standby = 20000;    //espero 20 segundos y vuelvo a resetear
            LED_ON;
            break;

        case PROG_WAIT_AUTOBAUD:
            if (Usart1_Autobaud())
            {
                Usart3Send("?");
                timer_standby = 10000;    //doy 10 segundos de timeout

                if (LED)
                    LED_OFF;
                else
                    LED_ON;
            }

            if (usart3_have_data)
            {
                usart3_have_data = 0;

                readed = ReadUsart3Buffer((unsigned char *)buff_local_bd, 126);
                //el puerto de la placa recien esta arrancando, tiene error, busco la 'S' del synchro
                for (unsigned char i = 0; i < readed; i++)
                {
                    if (*(buff_local_bd + i) == 'S')
                    {
                        i = readed;
                        Usart1Send("Synchronized\r\n");
                        prog_state = PROG_WAIT_SYNC_ON_PC;
                        timer_standby = 10000;    //doy 10 segundos mas
                    }
                }
            }

            if (!timer_standby)
                prog_state = PROG_RESET;
            
            break;

        case PROG_WAIT_SYNC_ON_PC:
            if (usart1_have_data)
            {
                usart1_have_data = 0;

                readed = ReadUsart1Buffer((unsigned char *)buff_local_bd, 126);
                //viene sucio con ?????
                for (unsigned char i = 0; i < readed; i++)
                {
                    if (*(buff_local_bd + i) == 'S')
                    {
                        i = readed;
                        Usart3Send("Synchronized\r\n");
                        prog_state = PROG_IN_ISP;
                        timer_standby = 10000;    //doy 10 segundos mas
                    }
                }
            }

            if (!timer_standby)
                prog_state = PROG_RESET;
            
            break;
            
        case PROG_IN_ISP:
            //Usart1 es la PC, Usart3 la placa a grabar
            if (usart1_have_data)
            {
                usart1_have_data = 0;
                readed = ReadUsart1Buffer((unsigned char *)buff_local_pc, 127);
                *(buff_local_pc + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
                *(buff_local_pc + readed + 1) = '\0';    //ajusto el '\0'
                Usart3Send(buff_local_pc);
                // Usart3SendUnsigned((unsigned char *) buff_local_pc, readed);
                timer_standby = 10000;    //doy 10 segundos mas
            }

            if (usart3_have_data)
            {
                usart3_have_data = 0;
                readed = ReadUsart3Buffer((unsigned char *)buff_local_bd, 127);
                *(buff_local_bd + readed) = '\n';    //cambio el '\0' por '\n' antes de enviar
                *(buff_local_bd + readed + 1) = '\0';    //ajusto el '\0'
                Usart1Send(buff_local_bd);
                // Usart1SendUnsigned((unsigned char *) buff_local_bd, readed);
                timer_standby = 10000;    //doy 10 segundos mas
            }

            if (!timer_standby)
                prog_state = PROG_RESET;

            if (!timer_led)
            {
                timer_led = 300;
                if (LED)
                    LED_OFF;
                else
                    LED_ON;
            }

            break;
        }
    }
    
    
#endif

#ifdef GLITCHER_ALWAYS_GLITCH
    
#endif
    
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
    
    if (timer_led)
        timer_led--;

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

        
        EXTI->PR |= 0x00000001;
    }
}

//--- end of file ---//

