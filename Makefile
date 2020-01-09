#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
##############################################################################################
# Start of default section
#
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
#AS   = $(TRGT)gcc -x assembler-with-cpp
AS   = $(TRGT)as
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m3

# List all default C defines here, like -D_DEBUG=1
#para el micro STM32F103RC
# DDEFS = -DSTM32F10X_HD
#para el micro STM32F103C8
DDEFS = -DSTM32F10X_MD

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR = ./src
CORELIBDIR = ./cmsis_core
STARTUPDIR = ./startup_src
LINKER = ./startup_src

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

#
# Define project name and Ram = 0/Flash = 1 mode here
PROJECT        = bluepill

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =


## Sources Files
SRC  = ./src/main.c
SRC += ./src/gpio.c
SRC += ./src/it.c
SRC += ./src/usart.c
SRC += ./src/timer.c
# SRC += ./src/flash_program.c
# SRC += ./src/adc.c


## Core Support
SRC += ./startup_src/syscalls.c
SRC += ./startup_src/startup_clocks.c
SRC += ./cmsis_core/core_cm3.c

# List ASM source files here
ASRC = ./startup_src/startup_stm32f10x_hd.s

# List all include directories here
UINCDIR = $(STARTUPDIR) \
          $(CORELIBDIR) \
          ./inc

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

# Define optimisation level here
# O1 optimiza size no significativo
# O2 size mas fuerte
# Os (size mas fuerte que O2)
# O3 el mas fuerte, seguro despues no corre
# O0 (no optimiza casi nada, mejor para debug)
OPT = -O0

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
LDSCRIPT = $(LINKER)/stm32_flash.ld
FULL_PRJ = $(PROJECT)_rom

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ADEFS   = $(DADEFS) $(UADEFS)
#OBJS  = $(ASRC:.s=.o) $(SRC:.c=.o)
LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -a=$(<:.s=.lst)

# SIN INFO DEL DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb   -fomit-frame-pointer -Wall -Wstrict-prototypes -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER + STRIP CODE
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DDEFS)

# SIN DEAD CODE, hace el STRIP
#LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

# SIN DEAD CODE, hace el STRIP + FLOAT = SOFT
LDFLAGS = $(MCFLAGS) -mthumb -lm --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

# CON DEAD CODE
#LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
#LDFLAGS = $(MCFLAGS) -mthumb -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

#
# makefile rules
#

assemblersources = $(ASRC)
sources = $(SRC)
OBJS  = $(ASRC:.s=.o) $(SRC:.c=.o)

objects = $(sources:.c=.o)
assobjects = $(assemblersources:.s=.o)

# all: $(OBJS) $(FULL_PRJ).elf $(FULL_PRJ).bin
# 	arm-none-eabi-size $(FULL_PRJ).elf

all: $(assobjects) $(objects) $(FULL_PRJ).elf $(FULL_PRJ).bin
	arm-none-eabi-size $(FULL_PRJ).elf
	gtags -q

$(assobjects): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

$(objects): %.o: %.c
	$(CC) -c $(CPFLAGS) -I. $(INCDIR) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN)  $< $@

flash:
	sudo openocd -f stm32f1_flash.cfg

gdb:
	sudo openocd -f stm32f1_gdb.cfg

reset:
	sudo openocd -f stm32f1_reset.cfg

clean:
	rm -f $(OBJS)
	rm -f $(FULL_PRJ).elf
	rm -f $(FULL_PRJ).map
	rm -f $(FULL_PRJ).hex
	rm -f $(FULL_PRJ).bin
#	rm $(SRC:.c=.c.bak)
	rm -f $(SRC:.c=.lst)
#   rm $(ASRC:.s=.s.bak)
	rm -f $(ASRC:.s=.lst)

# *** EOF ***
