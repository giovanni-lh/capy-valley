# Run from an MSYS2 terminal in the project root.

# make 			# build game.gba
# make run 		# build and launch in mGBA
# make clean 	# remove all build output

DEVKITPRO ?= /opt/devkitpro
DEVKITARM ?= $(DEVKITPRO)/devkitARM

CC	  	:= $(DEVKITARM)/bin/arm-none-eabi-gcc
OBJCOPY := $(DEVKITARM)/bin/arm-none-eabi-objcopy
GBAFIX 	:= $(DEVKITPRO)/tools/bin/gbafix

MGBA ?= '/c/Program Files/mGBA/mGBA.exe'

TARGET := game

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, build/%.o, $(SRCS))

ARCH := -mthumb -mthumb-interwork -mcpu=arm7tdmi -mtune=arm7tdmi
CFLAGS := $(ARCH) -O2 -Wall -fno-strict-aliasing -fomit-frame-pointer

SPECS := $(DEVKITARM)/arm-none-eabi/lib/gba-cart.specs
LDFLAGS := $(ARCH) -specs=$(SPECS) -Wl,-Map,$(TARGET).map

.PHONY: all run clean

all: $(TARGET).gba

$(TARGET).gba: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) $@ > /dev/null
	@echo "  ROM  $@ (done)"

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "  LD  $@"

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "  CC  $<"

build:
	mkdir -p build

run: all
	"$(MGBA)" $(CURDIR)/$(TARGET).gba

clean:
	rm -rf build $(TARGET).elf $(TARGET).gba $(TARGET).map
	@echo "  Cleaned done"