SRCS=$(wildcard *.cpp)
HEADERS=$(wildcard include/*.cpp)

export CROSS_COMPILE ?= mipsel-linux-gnu-

CXX=${CROSS_COMPILE}gcc
LD=${CROSS_COMPILE}ld
AR=${CROSS_COMPILE}ar
OBJCOPY=${CROSS_COMPILE}objcopy
OBJDUMP=${CROSS_COMPILE}objdump

COES=$(SRCS:.cpp=.coe)
ASM=$(SRCS:.cpp=.s) 
BASES=$(SRCS:.cpp=.base.bin)
EXTS=$(SRCS:.cpp=.ext.bin)
EXTS=$(SRCS:.cpp=.s)

CFLAGS=-c -ffreestanding -Wall -mxgot -fno-pic -EL -mips32 -O2 -I include -include common.h -include bootstrap.h
CXXFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=-static -EL -nostdlib --nmagic

LINKDER_DEFS_MEM=-DMEM_SIZE=0x00100000 -DTEXT_START=0x80000000
LINKDER_DEFS_BOOTROM=-DMEM_SIZE=0x00100000 -DTEXT_START=0xBFC00000 -DDATA_START=0x80700000

export AR CFLAGS

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
	$(OBJDUMP) -ald $@ > bootrom/$(basename $(basename $@)).s

%.ram.elf: %.o startup.o main.o linker.ram.ld lib/libtinyc.a
	mkdir -p ram
	$(LD) $(LDFLAGS) -T linker.ram.ld -o $@ $^
	$(OBJDUMP) -ald $@ > ram/$(basename $(basename $@)).s

linker.bootrom.ld: utility/linker.ld.S $(HEADERS)
	$(CXX) -E -P -DBOOT_FROM_BOOTROM $(LINKDER_DEFS_BOOTROM) $(CXXFLAGS) $< -o $@

linker.ram.ld: utility/linker.ld.S $(HEADERS)
	$(CXX) -E -P $(LINKDER_DEFS_MEM) $(CXXFLAGS) $< -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $<

main.o: utility/main.c
	$(CXX) $(CFLAGS) -o $@ $<

startup.o: utility/startup.s
	$(CXX) $(CFLAGS) -o $@ $<

lib/libtinyc.a:
	make -C lib libtinyc.a

clean:
	rm -rf ram/ bootrom/ *.ld *.o split_bin convert_bin
	make -C lib clean
