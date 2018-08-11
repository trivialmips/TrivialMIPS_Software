#include <elf.h>
#include <machine.h>
#include <memory_test.h>
#include <stdio.h>
#include <string.h>

const hword_t DEVICE_FLASH = 0x1;  // switch 1
const hword_t DEVICE_RAM = 0x2;    // switch 2
const hword_t DEVICE_UART = 0x4;   // switch 3
const hword_t BOOT_MODE = 0x8;     // switch 4
const hword_t CHECK_SRAM = 0x8000; // switch 16

extern void *_mem_start;

void boot_addr(void *addr) {
    printf("Booting from address %p...\n", addr);
    asm volatile("jr %0;" ::"r"(addr));
}

void check_overlap(void *addr) {
    if ((uint32_t)addr >= (uint32_t)_mem_start) {
        puts("WARNING: Program and bootloader memory overlap.");
    }
}

void wait_for_magic() {
    uint32_t count = 0;
    while (count < 4) {
        if (read_serial() == 0x23) ++count;
        else count = 0;
    }
}

void *copy_from_flash(void *addr) {
    auto *ehdr = reinterpret_cast<elf32_ehdr *>(addr);

    if (ehdr->e_ident[0] != ELF_MAGIC) {
        printf("No valid ELF magic found in address %p. Abort.\n",
               ehdr->e_ident);
        panic();
    }

    if (ehdr->e_machine != EM_MIPS_RS3_LE) {
        printf("Machine type %d of ELF file does not match the CPU. Abort.\n",
               ehdr->e_machine);
        panic();
    }

    auto *phdr = reinterpret_cast<elf32_phdr *>((uint32_t)addr + ehdr->e_phoff);
    auto *last_phdr = reinterpret_cast<elf32_phdr *>(
        (uint32_t)phdr + (ehdr->e_phentsize * ehdr->e_phnum));

    auto off = (phdr->p_vaddr - phdr->p_paddr);

    while (phdr < last_phdr) {
        printf("Copying %d bytes from offset %x to address %p\n",
               phdr->p_offset, phdr->p_filesz, phdr->p_paddr);
        auto *dest = reinterpret_cast<byte_t *>(phdr->p_paddr);
        auto *source =
            reinterpret_cast<byte_t *>((uint32_t)addr + phdr->p_offset);
        check_overlap(source + phdr->p_filesz);
        memcpy(dest, source, phdr->p_filesz);
        ++phdr;
    }

    return reinterpret_cast<void *>(ehdr->e_entry - off);
}

void *load_from_uart() {
    putstring("Send uint32 sequence: 0x23232323 OFFSET LENGTH");
    puts(" DATA...");
    
    wait_for_magic();

    auto *offset = reinterpret_cast<volatile byte_t *>(read_serial_word());
    word_t length = read_serial_word();
    auto *entry = reinterpret_cast<byte_t *>(read_serial_word());

    printf("Offset: %p, length: %d bytes, entry: %p. Start receiving data...\n",
           offset, length, entry);

    check_overlap((void *)(offset + length));

    for (size_t i = 0; i < length; ++i) {
        offset[i] = read_serial();
    }

    puts("Done receiving data.");

    return entry;
}

int _entry() {

    puts("Starting stage 1 bootloader...");
    auto switches = read_switches();
    if (switches & CHECK_SRAM) {
        puts("Starting memory test...");
        if (!test_memory()) {
            puts("Memory test failed. Abort.");
            panic();
        } else {
            puts("Memory test succeeded.");
        }
    }

    if (!(switches & BOOT_MODE)) { // boot mode
        putstring("Boot mode. Device: ");
        if (switches & DEVICE_FLASH) {
            puts("SPI Flash");
            // copy executable segment from flash to ram
            auto entry_addr = copy_from_flash(FLASH_START_ADDR);
            boot_addr(entry_addr);
        } else if (switches & DEVICE_UART) {
            puts("UART Receiver");
            // receive binary program fron uart to ram
            auto entry_addr = load_from_uart();
            boot_addr(entry_addr);
        } else if (switches & DEVICE_RAM) {
            puts("RAM");
            // do nothing
            boot_addr(MEM_START_ADDR);
        }
    } else { // dump mode
        puts("Dump mode.");
        while(true) {
            puts("Send uint32 sequence: 0x23232323 OFFSET LENGTH");
            wait_for_magic();
            auto *offset = reinterpret_cast<volatile byte_t *>(read_serial_word());
            word_t length = read_serial_word();

            printf("Offset: %p, length: %d bytes. Start sending data...",
                offset, length);

            for(size_t i = 0; i < length; ++i){
                write_serial(offset[i]);
            }

            puts("Done sending data");
        }
    }

    return 0; // make the compiler happy
}