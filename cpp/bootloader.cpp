#include <elf.h>
#include <machine.h>
#include <memory_test.h>
#include <stdio.h>
#include <string.h>
#include <trivial_mips.h>

const hword_t DEVICE_FLASH = 0x1;  // switch 1
const hword_t DEVICE_RAM = 0x2;    // switch 2
const hword_t DEVICE_UART = 0x4;   // switch 3
const hword_t BOOT_MODE = 0x8;     // switch 4
const hword_t CHECK_SRAM = 0x8000; // switch 16

extern byte_t _mem_start, _mem_end;
extern byte_t _mem_check_start, _mem_check_end;

void boot_addr(void *addr) {
    printf("Booting from address %p...\n", addr);
    asm volatile("jr %0;" ::"r"(addr));
}


void check_overlap(void *addr, uint32_t length) {
    auto start_addr = reinterpret_cast<byte_t* const>(addr);
    auto end_addr = start_addr + length;
    if ((start_addr >= &_mem_start && start_addr < &_mem_end) ||
        (end_addr >= &_mem_start && end_addr < &_mem_end)) {
        puts("ERROR: Program and bootloader memory overlap.");
        panic();
    }
}


void wait_for_magic() {
    uint32_t count = 0;
    while (count < 4) {
        if (read_serial() == 0x23)
            ++count;
        else
            count = 0;
    }
}


void *copy_from_flash(void *addr) {
    auto *ehdr = reinterpret_cast<elf32_ehdr *>(addr);

    if (ehdr->e_ident[0] != ELF_MAGIC) {
        printf("ERROR: No valid ELF magic found in address %p.\n",
               ehdr->e_ident);
        panic();
    }

    if (ehdr->e_machine != EM_MIPS) {
        printf("ERROR: Machine type %d of ELF file does not match the CPU.\n",
               ehdr->e_machine);
        panic();
    }

    auto *phdr = reinterpret_cast<elf32_phdr *>((uint32_t)addr + ehdr->e_phoff);
    auto *last_phdr = reinterpret_cast<elf32_phdr *>(
        (uint32_t)phdr + (ehdr->e_phentsize * ehdr->e_phnum));

    auto off = (phdr->p_vaddr - phdr->p_paddr);

    while (phdr < last_phdr) {
        printf("Copying %d bytes from offset %x to address 0x%p\n",
               phdr->p_filesz, phdr->p_offset, phdr->p_paddr);
        auto *dest = reinterpret_cast<byte_t *>(phdr->p_paddr);
        auto *source =
            reinterpret_cast<byte_t *>((uint32_t)addr + phdr->p_offset);
        check_overlap(dest, phdr->p_filesz);
        memcpy(dest, source, phdr->p_filesz);
        ++phdr;
    }

    return reinterpret_cast<void *>(ehdr->e_entry - off);
}


void *load_from_uart() {
    putstring("Send uint32 sequence: 0x23232323 OFFSET LENGTH");
    puts(" ENTRY DATA...");

    wait_for_magic();

    auto *offset = reinterpret_cast<volatile byte_t *>(read_serial_word());
    word_t length = read_serial_word();
    auto *entry = reinterpret_cast<byte_t *>(read_serial_word());

    printf("Offset: %p, length: %d bytes, entry: %p. Start receiving data...\n",
           offset, length, entry);

    check_overlap((void*)offset, length);

    for (size_t i = 0; i < length; ++i) {
        offset[i] = read_serial();
    }

    puts("Done receiving data.");

    return entry;
}


int _entry() {

    puts("Starting stage 1 bootloader...");

    printf("Bootloader used memory: from %x to %x\n", &_mem_start, &_mem_end);

    auto switches = get_switches();
    switches = get_switches(); // workaround for GPIO controller

    printf("Switch status is %x.\n", switches);
    if (switches & CHECK_SRAM) {
        if (!test_memory(&_mem_check_start, &_mem_check_end)) {
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
        } else {
            puts("Not Selected");
            panic();
        }
    } else { // dump mode
        puts("Dump mode.");
        while (true) {
            puts("Send uint32 sequence: 0x23232323 OFFSET LENGTH");
            wait_for_magic();
            auto *offset =
                reinterpret_cast<volatile byte_t *>(read_serial_word());
            word_t length = read_serial_word();

            printf("Offset: %p, length: %d bytes. Start sending data...",
                   offset, length);

            for (size_t i = 0; i < length; ++i) {
                write_serial(offset[i]);
            }

            puts("Done sending data");
        }
    }

    return 0; // make the compiler happy
}