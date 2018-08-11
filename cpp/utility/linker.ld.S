ENTRY(_start);
SECTIONS
{
    . = TEXT_START;

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
    }

    .data :
    {
        _data = .;
#ifdef BOOT_FROM_BOOTROM
        _stack = ABSOLUTE(DATA_START + MEM_SIZE - 32);
#else
        _stack = _data + MEM_SIZE - 32;
#endif
        *(.data)
        *(.data*)
        *(.eh_frame)
#ifdef BOOT_FROM_BOOTROM
        _gp = ABSOLUTE(DATA_START + 0x7ff0);
#else
        _gp = ALIGN(16) + 0x7ff0;  
#endif
        *(.got.plt) *(.got)
        *(.sdata) 
        *(.lit8) 
        *(.lit4)
        _data_end = .;
    }

    .sbss : 
    { 
        *(.sbss)
        *(.scommon)
    }
    
    .bss :
    {
        _bss = .;
        *(.dynbss)
        *(.bss)
        *(COMMON)
        _bss_end = .;
    }

    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
    }
}
