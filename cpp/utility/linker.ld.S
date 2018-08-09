ENTRY(_start);
SECTIONS
{
    . = TEXT_START;

    .text :
    {
        _text = .;
        *(.text.startup);
        *(.text);
        _text_end = .;
    }
#ifdef BOOT_FROM_BOOTROM
#else
    . = DATA_START;
#endif
    .data :
    {
        _data = .;
        *(.bss);
        *(.bss*);
        *(.data);
        *(.rodata*);
        *(COMMON);
#ifndef BOOT_FROM_BOOTROM
        _stack = _data + MEM_SIZE - 32;
        _gp = ALIGN(16) + 0x7ff0;
#else
        _stack = DATA_START + MEM_SIZE - 32;
        _gp = DATA_START + 0x7ff0;
#endif
        _data_end = .;
    }    
    /DISCARD/ :
    {
        *(.note*);
        *(.iplt*);
        *(.igot*);
        *(.rel*);
        *(.comment);
        /* add any unwanted sections spewed out by your version of gcc and flags here */    
    }
}
