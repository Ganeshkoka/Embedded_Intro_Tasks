// there's a default nordic file you can use (this pulls from that but cuts things down for the blinky example)
// this is the CMSIS (Cortex Microcontroller Software Interface Standard) system file

#include <stdint.h>
#include "nrf52840.h"

// HFINT (cheap RC clock for nRF) automatically starts when power on chip (like back up clock always ready)
// HFXO (crystal clock) more accurate

// Default clock frequency (64 MHz)
// SystemCoreClock is used by SysTick_Config in startup_nrf52840.c
// global variable telling other code frequency we're running at
uint32_t SystemCoreClock = 64000000;

// called by startup_nrf52840.c
void SystemInit(void) {
    // Enable the high-frequency crystal oscillator (HFXO) which is just a high frequency clock
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    
    // Wait for HFXO to start
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
    
    // Clear the event
    // otherwise won't know the next time HFXO will start
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    
    // Update SystemCoreClock variable
    // follows conventions to include it but redundant for our purposes
    // in real system, the clock might change
    SystemCoreClockUpdate();
}

void SystemCoreClockUpdate(void) {
    // In a full implementation, this would check the actual clock source
    // and calculate the real frequency. For our simple example, we know
    // we're using HFXO at 64MHz.
    SystemCoreClock = 64000000;
}