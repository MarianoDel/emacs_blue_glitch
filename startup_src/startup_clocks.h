//---------------------------------------------------------
// #### Part of Startup Files for STM32F103RC
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### STARTUP_CLOCKS.H ############################################
//---------------------------------------------------------

#ifndef _STARTUP_CLOCKS_H_
#define _STARTUP_CLOCKS_H_

#include "hard.h"        //for config pourpose

//--- Defines for conf clocks -------
#ifdef HSI_INTERNAL_RC
#define SYSCLK_FREQ_72MHz
// #define SYSCLK_FREQ_8MHz
#endif

#ifdef HSE_CRYSTAL_OSC
#define CRYSTAL_8MHZ
// #define CRYSTAL_12MHZ
#endif

//--- Exported Module Functions ----
void SystemInit (void);


#endif    /* _STARTUP_CLOCKS_H_ */

//--- end of file ---//
