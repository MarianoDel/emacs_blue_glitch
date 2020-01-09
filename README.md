# Placa de desarrollo Blue Pill Arduino STM32
micro STM32F103C8T6


Antes de empezar revisar seleccion del micro y configuracion
------------------------------------------------------------

* work area size y flash image
>stm32f1_flash.cfg

* work area size y flash image
>stm32f1_flash_lock.cfg

* work area size
>stm32f1_gdb.cfg

* work area size
>stm32f1_reset.cfg

* end of ram; stack size; memory lenght
>./startup_src/stm32_flash.ld

* elegir clock (xtal o rc)
>./src/hard.h

* elegir procesador definiciones en linea 66 de stm32f1xx.h -> pero modificar en Makefile
>./src/stm32f1xx.h
>.Makefile

