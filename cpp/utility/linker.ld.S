ENTRY(_start);

MEMORY
{
    BOOTROM  (rx)  : ORIGIN = 0xBFC00000, LENGTH = 8K
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

    .text :
    {
        _text = .;
        *(.text.startup)
        // . = ORIGIN(TEXT_AREA) + 0x0180;
        *(.text.exception)
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
