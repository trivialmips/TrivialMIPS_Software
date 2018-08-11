#include <elf.h>
#include <machine.h>
#include <memory_test.h>
#include <stdio.h>
#include <string.h>

const hword_t BOOT_FROM_FLASH = 0x1; // switch 1
const hword_t BOOT_FROM_SRAM = 0x2; // switch 2
const hword_t BOOT_FROM_UART = 0x4; // switch 3
const hword_t DUMP_FLASH = 0x8; // switch 4
const hword_t DUMP_SRAM = 0x10; // switch 5
const hword_t CHECK_SRAM = 0x8000; // switch 16

void boot_addr(void *addr) {
    printf("Booting from address %p...\n", addr);
    asm volatile("jr %0;" ::"r"(addr));
}

void *copy_from_elf(void *addr) {
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
        auto *dest = reinterpret_cast<void *>(phdr->p_paddr);
        auto *source =
            reinterpret_cast<void *>((uint32_t)addr + phdr->p_offset);
        memcpy(dest, source, phdr->p_filesz);
        ++phdr;
    }

    return reinterpret_cast<void *>(ehdr->e_entry - off);
}

int _entry() {
    printf("Hello TrivialMIPS!\n");
    auto switches = read_switches();
    if (switches & CHECK_SRAM) {
        printf("Starting memory test.\n");
        if (!test_memory()) {
            printf("Memory test failed, abort.\n");
            panic();
        } else {
            printf("Memory test succeeded.\n");
        }
    }

    if (switches & BOOT_FROM_FLASH) {
        // copy executable segment from flash to ram
        auto entry_addr = copy_from_elf(FLASH_START_ADDR);
        boot_addr(entry_addr);
    } else if (switches & BOOT_FROM_UART) {
        // receive binary program fron uart to ram
        boot_addr(MEM_START_ADDR);
    } else if (switches & BOOT_FROM_SRAM) {
        // do nothing
        boot_addr(MEM_START_ADDR);
    }
}