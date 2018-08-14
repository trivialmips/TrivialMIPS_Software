#include <stdio.h>
#include <memory_test.h>

word_t rand(word_t &rand_seed) {
	return rand_seed = (rand_seed * 16807L) % ((1u << 31) - 1);
}

template<typename Type>
bool do_test_memory(void* _start, void* _end) {
	word_t rand_seed;

	auto *start = reinterpret_cast<volatile Type *>(_start);
	auto *end = reinterpret_cast<volatile Type *>(_end);

	auto size = (uint32_t) _end - (uint32_t) _start;
	uint32_t progress;

	auto *mem = start;

	rand_seed = 23;

	while(mem < end) {
		*mem = rand(rand_seed);
		mem++;
	}

	mem = start;

	rand_seed = 23;

	while(mem < end) {
		progress = (uint32_t) mem - (uint32_t) _start;
		write_segment(progress * 100 / size);
		write_led((uint16_t)((uint32_t) mem));
		if(*mem != static_cast<Type>(rand(rand_seed))) {
			printf("Error at %x \n", mem);
			return false;
		}
		mem++;
	}

	write_led(0);
	write_segment(100);

    return true;
}

bool test_memory(void* start, void* end){

	printf("Starting memory test from 0x%p to 0x%p.\n", start, end);

    putstring("Testing memory by word...");
    if (!do_test_memory<word_t>(start, end)) return false;
	puts("OK!");

	putstring("Testing memory by half word...");
    if (!do_test_memory<hword_t>(start, end)) return false;
	puts("OK!");

	putstring("Testing memory by byte...");
    if (!do_test_memory<byte_t>(start, end)) return false;
	puts("OK!");

    return true;
}