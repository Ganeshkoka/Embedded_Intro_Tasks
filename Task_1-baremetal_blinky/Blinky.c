// main.c
#include <stdint.h>
#include "nrf52840.h"
#include "system_nrf52840.h"

// basically, two things happening in parallel. g_ticks increases every millisecond & main program stuck in delay loop until 501 reached
// when g_ticks reaches 501, main program is unstuck and can run again and executes whatever the next command was which is either turn on or off LED

// set up timer
// use volatile because variable is changed in an interrupt
static volatile uint32_t g_ticks = 0;

// 1 kHz SysTick ISR
// recall SysTick_Handler in vector table, we define it here to override "weak" default
// increements a counter every millisecond
void SysTick_Handler(void) {
    g_ticks++;
}

static void delay_ms(uint32_t ms) {
    uint32_t start = g_ticks;
    // nop means no operation - just wait until counter has reached desired time
    // helps with power consumption because literally does nothing for one cycle as opposed to constantly checking while loop
    while ((g_ticks - start) < ms) { __asm volatile("nop"); }
}

int main(void) {
    // Ensure core clock is set and SystemCoreClock is valid (64 MHz by default)
    SystemInit();

    // Configure SysTick to 1 kHz (1 ms tick)
    // SystemCoreClock is provided by system_nrf52840.c
    // systemcoreclock is 64 MHz by default (64 million cycles per second) --> divide by 1000 to get 64,000 cycles
    // SysTick counts to 64,000 then interrupt. Since counting at 64 MHz, 64 million cycles per second / 64000 cycles per interrupt = 1000 interrupts per second = 1 kHz = 1 interrupt every millisecond
    // under the hood it interrupts (calls handler) and increments g_ticks
    SysTick_Config(SystemCoreClock / 1000);

    // LED1 on nRF52840 DK -> P0.13, active-low
    const uint32_t LED_PIN = 13;

    // Configure P0.13 as output:
    // PIN_CNF[pin]: bit0=DIR (1=Output), other fields left default
    NRF_P0->PIN_CNF[LED_PIN] = (1u << 0);

    while (1) {
        // ON (active-low)
        // output clear makes it 0 - LED ON because active-low
        NRF_P0->OUTCLR = (1u << LED_PIN);
        delay_ms(500);

        // OFF
        NRF_P0->OUTSET = (1u << LED_PIN);
        delay_ms(500);
    }
}
