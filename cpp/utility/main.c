#include <stdio.h>
#include <string.h>
#include <machine.h>
#include <exception.h>

extern byte_t _bss, _bss_end;

void _main() {
    // clear bss section
    memset(&_bss, 0, (&_bss_end - &_bss));
    // call the actual function
    puts("=====Hello TrivialMIPS!=====");
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
    printf("An exception occurred, with epc %x and cause %d (%s).\n", _get_epc(), code, EXCEPTION_MESSAGES[code]);
}
