//---------------------------------------------------------
// #### Part of Startup Files for STM32F103RC
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// * .    - SystemInit(): Setups the system clock (System clock source, PLL Multiplier
// *                      factors, AHB/APBx prescalers and Flash settings). 
// *                      This function is called at startup just after reset and 
// *                      before branch to main program. This call is made inside
// *                      the "startup_stm32f10x_xx.s" file.
// *
// * .  After each device reset the HSI (8 MHz) is used as system clock source.
// *    Then SystemInit() function is called, in "startup_stm32f10x_xx.s" file, to
// *    configure the system clock before to branch to main program.
// *
// #### STARTUP_CLOCKS.C ############################################
//---------------------------------------------------------

#include "stm32f10x.h"
#include "startup_clocks.h"



#if (!defined HSI_INTERNAL_RC) && (!defined HSE_CRYSTAL_OSC)
#error	"No Oscillator Source Defined"
#endif

#ifndef HSE_CRYSTAL_OSC
#if (!defined SYSCLK_FREQ_72MHz) && (!defined SYSCLK_FREQ_8MHz)
#error	"No Oscillator Frequency Defined for RC"
#endif
#endif

#ifdef HSE_CRYSTAL_OSC
#if (!defined CRYSTAL_8MHZ) && (!defined CRYSTAL_12MHZ)
#error  "Crystal selected but not frequency defined"
#endif
#endif


void SystemInit (void)
{
    //seteo clocks, pll, flash latency
#ifdef HSE_CRYSTAL_OSC
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
    /* Enable HSE */    
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;  
    } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }  

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 2 wait state */
        FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
        FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

 
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

        /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                            RCC_CFGR_PLLMULL));
#ifdef CRYSTAL_8MHZ
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);
#endif

#ifdef CRYSTAL_12MHZ
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL6);
#endif
        

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }
    
        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
    }
#endif //HSE_CRYSTAL_OSC

#ifdef HSI_INTERNAL_RC

#ifdef SYSCLK_FREQ_72MHz
    //me aseguro que el PLL este deshabilitado
    RCC->CR &= ~RCC_CR_PLLON;

    //ajuste fino del clk HSI (internal RC)
    //en RCC->CR; HSITRIM[7:3] user trim value, HSICAL[15:8] es factoy default (read only)
    //todo a 0 baja freq (mas chico menos freq)
    RCC->CR &= ~RCC_CR_HSITRIM;
    RCC->CR |= 0x000000F8;    //max freq
    // RCC->CR |= 0x00000000;    //min freq
    
    
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
    //configuro el PLL
    /*  PLL configuration: PLLCLK = HSI/2 * 16 = 72 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLMULL16);
    
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
    

#endif    //SYSCLK_FREQ_72MHz

#ifdef SYSCLK_FREQ_8MHz
    //uso todo por defualt despues del reset
    //me aseguro que el PLL este deshabilitado
    RCC->CR &= ~RCC_CR_PLLON;

    // /* Enable Prefetch Buffer */
    // FLASH->ACR |= FLASH_ACR_PRFTBE;

    // /* Flash 2 wait state */
    // FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    // FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;

    // /* HCLK = SYSCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
    // /* PCLK2 = HCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
    
    // /* PCLK1 = HCLK */
    // RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    
#endif    //SYSCLK_FREQ_8MHz
    
#endif    //HSI_INTERNAL_RC
    
}

