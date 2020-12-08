

void put_int(int x);

int ipow(int x, int y) {
        int z = 1;
            while (y > 0) {
                        z = z * x;
                                y = y - 1;
                                    }
                return z;
}

int imax(int a, int b) {
        return a >= b ? a : b;
}

int main() {
        put_int(imax(ipow(2, 3), ipow(3, 2)));
            put_int(imax(ipow(2, 5), ipow(5, 2)));
                return 0;
}

