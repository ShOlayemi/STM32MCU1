# STM32F407 Bare-Metal Drivers

![CI](https://github.com/ShOlayemi/STM32MCU1/actions/workflows/ci.yml/badge.svg)

Register-level GPIO and SPI drivers for the **STM32F407 Discovery** board, written in C without ST's HAL or LL libraries. Every peripheral is reached through a hand-written device header (base addresses, register structs, bit positions) and the drivers sit directly on top of it.

The main project is [`stm32f4xx_drivers/`](stm32f4xx_drivers). The other folders are earlier warm-up exercises (see [Repository layout](#repository-layout)).

**Skills shown:** bare-metal C with no HAL/LL; memory-mapped register access through a hand-written device header; NVIC and EXTI interrupt configuration; interrupt-driven SPI with a TX/RX state machine; an SPI master-slave command protocol with an Arduino; on-target debugging with ST-LINK, semihosting and a logic analyser.

## What's in it

**Device header** ([`stm32f407xx.h`](stm32f4xx_drivers/drivers/Inc/stm32f407xx.h))
- Memory map for the AHB1/APB1/APB2 buses and peripheral base addresses
- `volatile` register structs for GPIO, RCC, EXTI, SYSCFG and SPI
- Clock-enable / reset macros, IRQ numbers, NVIC register addresses and priority levels

**GPIO driver** ([`stm32f407xx_gpio_driver.c`](stm32f4xx_drivers/drivers/Src/stm32f407xx_gpio_driver.c))
- Peripheral clock control, init / de-init through RCC reset
- Input, output, alternate-function and analog modes; speed, pull-up/pull-down and output type
- Read / write / toggle a pin or a whole port
- External interrupts: falling, rising or both edges, wired through EXTI, SYSCFG port selection and the NVIC (enable, disable, priority)

**SPI driver** ([`stm32f407xx_spi_driver.c`](stm32f4xx_drivers/drivers/Src/stm32f407xx_spi_driver.c))
- Master / slave, full-duplex, half-duplex and simplex RX-only bus configurations
- Clock prescaler, 8/16-bit frames, CPOL/CPHA, software or hardware slave management (SSI / SSOE)
- Blocking send and receive
- Interrupt-driven send and receive with a TX/RX state machine, overrun handling and an application event callback

## Hardware

- STM32F407G-DISC1 (STM32F407VGT6, Cortex-M4), 16 MHz HSI default clock
- Arduino Uno as SPI slave for demos 07 to 09
- Logic analyser recommended for checking SPI traffic

### SPI wiring (demos 06 to 09)

SPI2 on port B, alternate function 5:

| STM32 pin | Function  | Arduino Uno pin |
|-----------|-----------|-----------------|
| PB13      | SPI2_SCLK | D13 (SCK)       |
| PB15      | SPI2_MOSI | D11 (MOSI)      |
| PB14      | SPI2_MISO | D12 (MISO)      |
| PB12      | SPI2_NSS  | D10 (SS)        |
| PD6       | EXTI input, "data ready" from slave (demo 09) | Slave interrupt output |
| GND       | Ground    | GND             |

The Arduino runs at 5 V and the STM32 at 3.3 V; use a level shifter on the lines the Arduino drives.

## Demos

All live in [`stm32f4xx_drivers/drivers/Src/`](stm32f4xx_drivers/drivers/Src).

| # | File | What it shows |
|---|------|---------------|
| 01 | `01led_toggle.c` | Toggle the on-board LED (PD15) in a busy loop |
| 02 | `02led_button.c` | User button (PA0) toggles the on-board LED |
| 03 | `03led_button_ext.c` | External button on PB12 toggles an external LED on PA8 |
| 03b | `03led_button_ext2_PUres.c` | Same, using the internal pull-up instead of an external resistor |
| 04 | `04button_interrupt.c` | Button on PD5 fires a falling-edge EXTI interrupt (EXTI9_5) that toggles PD12 |
| 06 | `06spi_tx_testing.c` | SPI2 master sends "Hello world", checked on a logic analyser |
| 07 | `07spi_txonly_arduino.c` | Button press sends a length byte and a string to an Arduino slave |
| 08 | `08spi_cmd_handling.c` | Command/response protocol with the Arduino: LED control, analog read, LED status, print and ID read, with results printed over semihosting |
| 09 | `09spi_message_rcv_it.c` | Arduino signals data-ready on PD6; STM32 reads the message with the interrupt-driven SPI receive and prints it |

## Building and running

1. Open the folder as a workspace in **STM32CubeIDE** (built with GNU Tools for STM32 13.3.rel1) and import `stm32f4xx_drivers`.
2. Only one demo can be built at a time, since each has its own `main()`. Exclude the others from the build (right-click a file > *Resource Configurations* > *Exclude from Build*). Demo 09 is the one currently enabled, and `Src/main.c` is excluded.
3. Build, connect the Discovery board over ST-LINK and start a debug session.

For the demos that use `printf` (08, 09), see [`SEMIHOSTING_CMD.txt`](SEMIHOSTING_CMD.txt): link with `--specs=rdimon.specs -lc -lrdimon`, add `monitor arm semihosting enable` to the debug startup commands, and call `initialise_monitor_handles()` at the start of `main()`.

## Repository layout

| Folder | Contents |
|--------|----------|
| `stm32f4xx_drivers/` | The driver library and demos above |
| `STM32F407xx_Drivers/` | First version of the device header |
| `PERIADC/` | Enabling the ADC1 clock and writing ADC CR1 through raw pointers |
| `002SampleApp/` | C warm-up (array fill, bubble sort) run on target |
| `CLOCK/`, `PeriClockEn/` | CubeMX `.ioc` clock configuration experiments |

## Background and what I added

I learned register-level driver design through FastBit Embedded Brain Academy's *Mastering Microcontroller and Embedded Driver Development* course, then wrote my own drivers, structured around my own STM32F407 Discovery board. After every module I rewrote the driver myself and tested everything on my own hardware.

## Known issues and next steps

- I2C and USART have base addresses in the header but no driver yet.

Planned: add I2C and USART drivers and a small sensor project that uses them.
