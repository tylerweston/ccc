#include <cstdint>
#include <cstdio>

extern "C" {

	void putint(int32_t x) {
		printf("putint: %d\n", x);
	}

}
