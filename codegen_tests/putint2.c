
void putint(int x);

float foo(float a, float b) {
    return a * a + b * b;
}

int main() {
    putint((int) (foo(2.0, 4.0) / 2.0) - 2);
    return 0;
}
