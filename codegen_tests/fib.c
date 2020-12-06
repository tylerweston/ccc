

void put_int(int x);

int fib1(int n) {
        if (n <= 1) {
                    return n;
                        }
            return fib1(n - 1) + fib1(n - 2);
}

int fib2(int n) {
        int a = 0;
            int b = 1;
                for (int i = 0; i < n; i = i + 1) {
                            int c = a + b;
                                    a = b;
                                            b = c;
                                                }
                    return a;
}

int main() {
        for (int i = 0; i < 10; i = i + 1) {
                    put_int(fib1(i));
                            put_int(fib2(i));
                                }
            return 0;
}

