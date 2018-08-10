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


byte_t read_serial(){
	while (!(read<byte_t>(UART_STATUS_ADDR) & UART_DATA_READY));
	return read<byte_t>(UART_DATA_ADDR);
}

void write_serial(byte_t data){
	while (!(read<byte_t>(UART_STATUS_ADDR) & UART_CLEAR_TO_SEND));
	write<byte_t>(UART_DATA_ADDR, data);
}

void write_led(hword_t data){
	write<hword_t>(LED_ADDR, data);
}

void write_segment(hword_t data, bool decode){
	if (decode) {
		write<byte_t>(NUM_ADDR, (byte_t) data);
	} else {
		write<word_t>(NUM_ADDR, 0x80000000 | data);
	}
}

hword_t read_switches(){
	return read<hword_t>(SWITCHES_ADDR);
}