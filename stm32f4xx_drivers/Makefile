# Build every demo in drivers/Src (files starting with a digit) as its own .elf
.RECIPEPREFIX = >
CC      = arm-none-eabi-gcc
SIZE    = arm-none-eabi-size
MCU     = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS  = $(MCU) -std=gnu11 -O0 -g -Wall -ffunction-sections -fdata-sections -Idrivers/Inc
LDFLAGS = $(MCU) -T STM32F407VGTX_FLASH.ld --specs=nano.specs --specs=rdimon.specs -Wl,--gc-sections
BUILD   = build

DRIVERS = $(wildcard drivers/Src/stm32f407xx_*_driver.c)
STARTUP = Startup/startup_stm32f407vgtx.s
DEMOS   = $(wildcard drivers/Src/[0-9]*.c)
ELFS    = $(patsubst drivers/Src/%.c,$(BUILD)/%.elf,$(DEMOS))

all: $(ELFS)
> $(SIZE) $(ELFS)

$(BUILD)/%.elf: drivers/Src/%.c $(DRIVERS) $(STARTUP) | $(BUILD)
> $(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(BUILD):
> mkdir -p $(BUILD)

clean:
> rm -rf $(BUILD)

.PHONY: all clean
