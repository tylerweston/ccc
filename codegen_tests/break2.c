
void putint(int x);

int foo() {
    for (int i = 0; i < 10; i += 1) {
        for (int j = 0; j < 10; j += 1) {
            if (j == 3) {
                break;
            }
            putint(i + j);
        }
        if (i == 3) {
            break;
        }
    }
    return 0;
}

int main() {
    return foo();
}
