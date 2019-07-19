#include <machine.h>

template<typename T>
T read(void *addr){
    auto *ptr = reinterpret_cast<volatile T *const>(addr);
    return *ptr;
}

template<typename T>
void write(void *addr, T data){
    auto *ptr = reinterpret_cast<volatile T *>(addr);
    *ptr = data;
}

#define GENERATE_READ_FUNC(TYPE) TYPE##_t read_##TYPE(void* addr){ \
    return read<TYPE##_t>(addr); \
}

#define GENERATE_WRITE_FUNC(TYPE) void write_##TYPE(void* addr, TYPE##_t data){ \
    write<TYPE##_t>(addr, data); \
}

#define RUN_FOR_ALL_TYPE(FUNC) FUNC(byte) \
    FUNC(hword) \
    FUNC(word) \
    FUNC(dword)

RUN_FOR_ALL_TYPE(GENERATE_READ_FUNC)
RUN_FOR_ALL_TYPE(GENERATE_WRITE_FUNC)

void init_serial() {
	// Turn off the FIFO
	write_byte(UART_FCR_ADDR, 0);
	// Set speed; requires DLAB latch
	write_byte(UART_LCR_ADDR, 0x80);
	write_byte(UART_DLL_ADDR, (byte_t) (115200 / 9600));
	write_byte(UART_DLM_ADDR, 0);
	// 8 data bits, 1 stop bit, parity off; turn off DLAB latch
	write_byte(UART_LCR_ADDR, ~0x80 & 0x03);
	// No modem controls
	write_byte(UART_MCR_ADDR, 0);
	// No interrupts
	write_byte(UART_IER_ADDR, 0);
}

byte_t read_serial(){
	while (!(read_byte(UART_LSR_ADDR) & 0x01));
	return read_byte(UART_DAT_ADDR);
}

void write_serial(byte_t data){
	while (!(read_byte(UART_LSR_ADDR) & 0x40));
	write_byte(UART_DAT_ADDR, data);
}

word_t read_serial_word(){
	word_t word = 0;
	word |= read_serial();
	word |= read_serial() << 8;
	word |= read_serial() << 16;
	word |= read_serial() << 24;
	return word;
}

void write_led(hword_t data){
	write_hword(LED_ADDR, data);
}

void write_segment(word_t data){
	write_word(NUM_ADDR, data);
}

word_t read_switches(){
	return read_word(SWITCHES_ADDR);
}
