// IMPORTANT FOR UNDERSTANDING:
// only .data section is copied to RAM because modern microcontrollers can Execute-In-Place (XIP) meaning they can execute code directly from flash. But they can't write easily (requires more voltage, etc.), so data needs ot be in RAM!



// startup_nrf52840.c
// standard C library header providing fixed-width integer types; different microcontrollers have different widths (8, 16, 32, 64 bits, signed or unsigned)
#include <stdint.h>

// linker is program that takes all compiled object files (.o files) and decides in memory where should go, resolves connection between files, creates final executable
// linker.ld is "linker script" that tells linker how much memory you have (RAM and Flash), where different types of data should go, how to organize program in memory

// Symbols from linker.ld
// extern means that the symbol is defined in another file (in this case, linker.ld)
// unsigned long is 32-bit unsigned integer
extern unsigned long _estack; // end of stack (top of RAM), where stack pointer starts
extern unsigned long _sidata;  // start of .data in flash, where initialized data is stored in flash (source initial data)
extern unsigned long _sdata;   // start of .data in RAM, where data should be copied to in RAM
extern unsigned long _edata;   // end of .data in RAM

// uninitialized data section in RAM, initialized to 0 automically because of C protocal, startup code writes zeros to the entire .bss region
extern unsigned long _sbss;    // start of .bss, start of uninitialized data section in RAM
extern unsigned long _ebss;    // end of .bss, end of uninitialized data section in RAM


//declarations telling compiler these functions exist somewhere else
int main(void);
void SystemInit(void);


// Default weak handlers, this is default unless someone provides their own implementation
void Default_Handler(void); // this just loops forever (so all the interrupt handlers call this)
void Reset_Handler(void);

// Weak alias all IRQs (Interrupt ReQuest) to Default_Handler unless you define them
#define WEAK __attribute__((weak)) // don't actually need this, but it's good practice. You can specifically mark handlers as weak with this
#define ALIAS(x) __attribute__((weak, alias(#x)))


// Interrupt Handlers, standard ARM Cortex-M4 exception handlers
void NMI_Handler(void)                ALIAS(Default_Handler); // Non-Maskable Interrupt, cannot be disabled (security, power disruptions, etc.)
void HardFault_Handler(void)          ALIAS(Default_Handler); // catches serious system errors
void MemoryManagement_Handler(void)   ALIAS(Default_Handler); // handles memory access violations
void BusFault_Handler(void)           ALIAS(Default_Handler); // handles bus errors like accessing invalid address
void UsageFault_Handler(void)         ALIAS(Default_Handler); // handles program errors like division by zero
void SVC_Handler(void)                ALIAS(Default_Handler); // System Service Call
void DebugMon_Handler(void)           ALIAS(Default_Handler); // debugging
void PendSV_Handler(void)             ALIAS(Default_Handler); // system-level task switching, switching between programs A and B (saving results from one when it finishes)
void SysTick_Handler(void)            ALIAS(Default_Handler); // system timer interrupt

// Minimal vector table (start of flash, provided in .isr_vector)
__attribute__((section(".isr_vector"))) // section is a linker directive that tells linker to put this in the .isr_vector section
void (* const g_pfnVectors[])(void) = { // array of function pointers to the interrupt handlers
    // order is CRITICAL, processor expects specific handlers at specific addresses
    (void (*)(void))(&_estack), // Initial stack pointer (position 0)
    Reset_Handler,              // Reset (position 1, etc. position EXACTLY as defined here)
    NMI_Handler,                // NMI
    HardFault_Handler,          // HardFault
    MemoryManagement_Handler,   // MemManage
    BusFault_Handler,           // BusFault
    UsageFault_Handler,         // UsageFault
    0, 0, 0, 0,                 // Reserved for future use but must be kept as NULL for compatibility
    SVC_Handler,                // SVCall
    DebugMon_Handler,           // Debug monitor
    0,                          // Reserved
    PendSV_Handler,             // PendSV
    SysTick_Handler,            // SysTick
    // You can add device-specific IRQs here if you need them.
};


// FIRST code that runs after power on
void Reset_Handler(void) {
    // Copy .data from flash to RAM
    unsigned long *src = &_sidata;
    unsigned long *dst = &_sdata;
    while (dst < &_edata) { *dst++ = *src++; }

    // Zero .bss (uninitialized data section in RAM)
    for (dst = &_sbss; dst < &_ebss; ) { *dst++ = 0; }

    // Optionally call SystemInit() here (also called in main)
    // SystemInit();

    // Call main
    (void)main();

    // If main returns, trap
    while (1) { }
}

// just stopping code isn't informative though
// you typically use a debuggger to see where code stopped, add LED patterns or debug output (print), UART/Serial output, etc.
void Default_Handler(void) {
    while (1) { }
}
