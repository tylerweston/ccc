#include <cstdint>
#include <cstdio>

extern "C" {

	void put_int(int32_t x) {
		printf("putint: %d\n", x);
	}

	void putint(int32_t x) {
		printf("putint: %d\n", x);
	}

	void putascii(int32_t x) {
		printf("%c", (char) x);
	}
}
