#include <stdio.h>
#include <memory_test.h>

word_t rand(word_t &rand_seed) {
	return rand_seed = (rand_seed * 16807L) % ((1u << 31) - 1);
}

template<typename Type>
bool do_test_memory() {
	word_t rand_seed;

	auto *mem = reinterpret_cast<volatile Type *>(MEM_START_ADDR);

	rand_seed = 23;
	for(int i = 0; i != 8 * 1024 * 1024 / sizeof(Type); ++i)
		mem[i] = rand(rand_seed);

	rand_seed = 23;
	for(int i = 0; i != 8 * 1024 * 1024 / sizeof(Type); ++i){
		if(mem[i] != static_cast<Type>(rand(rand_seed))) {
			printf("Error at %x \n\n", MEM_START_ADDR + i * sizeof(Type));
			return false;
		}
    }

    return true;
}

bool test_memory(){
    printf("Testing memory by word!\n");
    if (!do_test_memory<word_t>()) return false;
	printf("OK!\n");

	printf("Testing memory by half word!\n");
    if (!do_test_memory<hword_t>()) return false;
	printf("OK!\n");

	printf("Testing memory by byte!\n");
    if (!do_test_memory<byte_t>()) return false;
	printf("OK!\n");

    return true;
}