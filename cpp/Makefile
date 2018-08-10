SRCS=$(wildcard *.cpp)
HEADERS=$(wildcard include/*.cpp)

export CROSS_COMPILE ?= mipsel-linux-gnu-

CC=${CROSS_COMPILE}gcc
CXX=${CROSS_COMPILE}g++
LD=${CROSS_COMPILE}ld
AR=${CROSS_COMPILE}ar
OBJCOPY=${CROSS_COMPILE}objcopy
OBJDUMP=${CROSS_COMPILE}objdump

COES=$(SRCS:.cpp=.coe)
ASM=$(SRCS:.cpp=.s) 
BASES=$(SRCS:.cpp=.base.bin)
EXTS=$(SRCS:.cpp=.ext.bin)
EXTS=$(SRCS:.cpp=.s)

COMP_FLGAS=-nostdinc -nostdlib -ffreestanding -Wall -mxgot -fno-builtin -fno-PIC -fno-PIE -mno-abicalls -g -EL -mhard-float -mips32 -O2 -I include -include common.h -include bootstrap.h
CXXFLAGS=$(COMP_FLGAS) -std=c++11
CFLAGS=$(COMP_FLGAS) -Wno-implicit-function-declaration

LDFLAGS=-static -EL -nostdlib --nmagic

LINKDER_DEFS_MEM=-DMEM_SIZE=0x00100000 -DTEXT_START=0x80000000
LINKDER_DEFS_BOOTROM=-DMEM_SIZE=0x00100000 -DTEXT_START=0xBFC00000 -DDATA_START=0x80700000

export CC CXX AR CFLAGS CXXFLAGS

all: bootrom sram

bootrom: $(COES)

sram: $(BASES)  #$(EXTS)

%.coe: %.bootrom.bin convert_bin
	mkdir -p bootrom
	./convert_bin $< bootrom/$@

%.base.bin: %.ram.bin split_bin
	./split_bin $< ram/$(basename $(basename $<)).ext.bin ram/$(basename $(basename $<)).base.bin

convert_bin: ../utility/convert_bin.c
	gcc -Wall -O2 -o $@ $<

split_bin: ../utility/split_bin.cpp
	g++ -Wall -Werror -o $@ $<

%.bootrom.bin: %.bootrom.elf
	$(OBJCOPY) -O binary -j .text $< $@

%.ram.bin: %.ram.elf
	$(OBJCOPY) -O binary -j .text -j .data $< $@

%.bootrom.elf: %.o startup.o main.o linker.bootrom.ld lib/libtinyc.a
	mkdir -p bootrom
	$(LD) $(LDFLAGS) -T linker.bootrom.ld -o $@ $^
	$(OBJDUMP) -aldS $@ > bootrom/$(basename $(basename $@)).s

%.ram.elf: %.o startup.o main.o linker.ram.ld lib/libtinyc.a
	mkdir -p ram
	$(LD) $(LDFLAGS) -T linker.ram.ld -o $@ $^
	$(OBJDUMP) -aldS $@ > ram/$(basename $(basename $@)).s

linker.bootrom.ld: utility/linker.ld.S $(HEADERS)
	$(CC) -E -P -DBOOT_FROM_BOOTROM $(LINKDER_DEFS_BOOTROM) $(CFLAGS) $< -o $@

linker.ram.ld: utility/linker.ld.S $(HEADERS)
	$(CC) -E -P $(LINKDER_DEFS_MEM) $(CFLAGS) $< -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: utility/main.c
	$(CC) $(CFLAGS) -c -o $@ $<

startup.o: utility/startup.s
	$(CC) $(CFLAGS) -c -o $@ $<

lib/libtinyc.a:
	make -C lib libtinyc.a

clean:
	rm -rf ram/ bootrom/ *.ld *.o split_bin convert_bin
	make -C lib clean
