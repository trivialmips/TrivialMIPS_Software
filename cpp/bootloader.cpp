#include <stdio.h>
#include <machine.h>
#include <memory_test.h>

const hword_t BOOT_FROM_FLASH = 0x0;
const hword_t BOOT_FROM_SRAM = 0x1;
const hword_t BOOT_FROM_UART = 0x2;
const hword_t DUMP_FLASH = 0x4;
const hword_t DUMP_SRAM = 0x8;
const hword_t CHECK_SRAM = 0x16;

void boot_ram(){
    asm volatile(
        "lui $t1, 0x8000;\n\t"
        "jr $t1;"
    );
}


int _entry(){
    printf("Hello TrivialMIPS!\n");
    auto switches = read_switches();
    if (switches & CHECK_SRAM){
        printf("Starting memory test.\n");
        if (!test_memory()){
            printf("Memory test failed, abort.\n");
            panic();
        } else {
            printf("Memory test succeeded.\n");
        }
    }

    if (switches & BOOT_FROM_FLASH) {
        // copy executable segment from flash to ram
        boot_ram();
    } else if (switches & BOOT_FROM_UART) {
        // receive binary program fron uart to ram
        boot_ram();
    } else if (switches & BOOT_FROM_SRAM) {
        // do nothing
        boot_ram();
    }

}