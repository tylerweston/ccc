#include <cstdint>
#include <cstdio>

extern "C" {

	void putint(int64_t x) {
		printf("putint: %ld\n", x);
	}

}
