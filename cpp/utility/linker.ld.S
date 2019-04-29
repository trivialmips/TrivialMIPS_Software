ENTRY(_start);

MEMORY
{
    BOOTROM  (rx)  : ORIGIN = 0xBFC00000, LENGTH = 16K
    RAM_BASE (rwx) : ORIGIN = 0x80000000, LENGTH = 7M
    RAM_HIGH (rwx) : ORIGIN = 0x80700000, LENGTH = 1M
}

#ifdef CODE_INTO_BOOTROM
#define TEXT_AREA BOOTROM
#define DATA_AREA RAM_HIGH
#else
#define TEXT_AREA RAM_BASE
#define DATA_AREA RAM_BASE
#endif

SECTIONS
{
    . = ORIGIN(TEXT_AREA);

    _mem_start = ORIGIN(DATA_AREA);
    _mem_end = ORIGIN(DATA_AREA) + LENGTH(DATA_AREA);

#ifdef CODE_INTO_BOOTROM
    _mem_avail_start = ORIGIN(RAM_BASE);
    _mem_avail_end = ORIGIN(RAM_BASE) + LENGTH(RAM_BASE);
#else
    _mem_avail_start = _stack;
    _mem_avail_end = ORIGIN(RAM_HIGH) + LENGTH(RAM_HIGH);
#endif

    .text :
    {
        _text = .;
        *(.text.startup)
        *(.text*)
        *(.rodata*)
        *(.reginfo)
        *(.init)
        *(.stub)
        *(.gnu.warning)
        *(.MIPS.abiflags)
        _text_end = .;
    } > TEXT_AREA

    .data :
    {
        _data = .;
        _stack = _data + MEM_SIZE - 32;
        *(.data)
        *(.data*)
        *(.eh_frame)
        _gp = ALIGN(16) + 0x7ff0;  
        *(.got.plt) *(.got)
        *(.sdata) 
        *(.lit8) 
        *(.lit4)
        _data_end = .;
    } > DATA_AREA

    .sbss : 
    { 
        *(.sbss)
        *(.scommon)
    } > DATA_AREA
    
    .bss :
    {
        _bss = .;
        *(.dynbss)
        *(.bss)
        *(COMMON)
        _bss_end = .;
    } > DATA_AREA

    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    }
}
