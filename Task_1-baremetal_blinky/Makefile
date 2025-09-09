# Makefile
# responsible for building the project

# Note: Using soft float ABI since we don't need FPU for this simple example
# For hardware FP later: switch to -mfloat-abi=hard -mfpu=fpv4-sp-d16

# Path to Nordic SDK headers
NRFX_MDK = /opt/nordic/ncs/v3.0.2/modules/hal/nordic/nrfx/mdk

# Toolchain (installed via brew)
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Flags
# -mcpu=cortex-m4: Tell compiler we're using ARM Cortex-M4
# -mthumb: Use Thumb instruction set (more compact ARM instructions)
# -mfloat-abi=soft: Don't use hardware floating point
# -O0: No optimization (good for debugging)
# -g3: Include debug information
# -I$(NRFX_MDK): Where to find Nordic's header files
CFLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=soft \
         -O0 -g3 -ffunction-sections -fdata-sections \
         -I$(NRFX_MDK) \
         --specs=nosys.specs --specs=nano.specs

# Linker flags
# -T linker.ld: Use our linker script
# -nostartfiles: Don't use standard startup files (we have our own!)
# --gc-sections: Remove unused code sections (makes binary smaller)
LDFLAGS = -T linker.ld -nostartfiles -Wl,--gc-sections

# Sources
# lists all our source files and creates object file names
SRCS = startup_nrf52840.c system_nrf52840.c Blinky.c
OBJS = $(SRCS:.c=.o)

# Outputs
# compile each .c file to .o file
# link them together into blinky.elf
# convert to blinky.hex format needed for flashing
ELF = blinky.elf
HEX = blinky.hex

all: $(HEX)

$(ELF): $(OBJS) linker.ld
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

clean:
	rm -f $(OBJS) $(ELF) $(HEX)

# Print the path it found for NRFX_MDK
path:
	@echo "NRFX_MDK path: $(NRFX_MDK)"