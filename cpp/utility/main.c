#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <exception.h>

extern byte_t _bss, _bss_end;

void _main() {

    init_serial();

    puts("*****TrivialMIPS Bare Metal System*****");

    // clear bss section if needed
    auto bss_size = (uint32_t) &_bss_end - (uint32_t) &_bss;
    if (bss_size > 0) {
        printf("Filling .bss section with 0, offset: 0x%x, size: %d bytes.\n", &_bss, bss_size);
        memset(&_bss, 0, bss_size);
    }

    // call the actual function
    int result = _entry();
    if (result == 0) {
        puts("Program exited normally.");
    } else {
        printf("Program exited abnormally with code %d.\n", result);
    }
}

word_t _get_epc() {
    word_t n;
    asm(
        "mfc0 %0, $14\n\t"
        :"=r"(n)
        );
    return n;
}

word_t _get_cause() {
    word_t n;
    asm(
        "mfc0 %0, $13\n\t"
        :"=r"(n)
        );
    return n;
}

void _exception_handler() {
    word_t code = (_get_cause() >> 2) & 0xF;
    auto epc = _get_epc();
    printf("An exception occurred, with epc 0x%x and cause %d (%s).\n", epc, code, EXCEPTION_MESSAGES[code]);
    write_led((uint16_t) epc);
}
