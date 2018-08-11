#include <stdio.h>
#include <string.h>
#include <bootstrap.h>

extern void *_bss, *_bss_end;

void _main() {
    // clear bss section
    memset(_bss, 0, (_bss_end - _bss));
    // call the actual function
    int result = _entry();
    if (result == 0) {
        printf("Program exited normally.\n");
    } else {
        printf("Program exited abnormally with code %d.\n", result);
    }
}

unsigned int _get_epc() {
    unsigned int n;
    asm(
        "mfc0 %0, $14\n\t"
        :"=r"(n)
        );
    return n;
}

unsigned int _get_cause() {
    unsigned int n;
    asm(
        "mfc0 %0, $13\n\t"
        :"=r"(n)
        );
    return n;
}

void _exception_handler() {
    printf("An exception occurred, with epc %x and cause %x.\n", _get_epc(), _get_cause());
    return;
}
