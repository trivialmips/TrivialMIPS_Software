#ifndef __MACHINE_H_
#define __MACHINE_H_

#include <stdint.h>

typedef uint8_t     byte_t;
typedef uint16_t    hword_t;
typedef uint32_t    word_t;
typedef uint64_t    dword_t;

#define CPU_COUNT_PER_US        1000 // tentative

#define ADDR(addr)              ((void*)addr)

#define SWITCHES_ADDR           ADDR(0xA6000000)
#define LED_ADDR                ADDR(0xA6000008)
#define NUM_ADDR                ADDR(0xA6000004)
#define UART_STATUS_ADDR		ADDR(0xA3000000)
#define UART_DATA_ADDR			ADDR(0xA3000004)

#define TIMER_CYCLE_ADDR		ADDR(0xA4000004)
#define TIMER_MICROSEC_ADDR		ADDR(0xA4000000)

#define UART_DATA_READY         2
#define UART_CLEAR_TO_SEND      1

#ifdef __cplusplus
extern "C" {
#endif

byte_t read_byte(void* addr);

hword_t read_hword(void* addr);

word_t read_word(void* addr);

dword_t read_dword(void* addr);

void write_byte(void* addr, byte_t data);

void write_hword(void* addr, hword_t data);

void write_word(void* addr, word_t data);

void write_dword(void* addr, dword_t data);


byte_t read_serial();

void write_serial(byte_t data);

void write_led(hword_t data);

#ifdef __cplusplus
void write_segment(hword_t data, bool decode = true);
#else
void write_segment(hword_t data, _Bool);
#endif

hword_t read_switches();

#ifdef __cplusplus
}
#endif

#endif
