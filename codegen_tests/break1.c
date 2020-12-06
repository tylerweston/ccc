
void putint(int x);

int main() {
    for (int i = 0; i < 10; i += 1) {
        putint(i);
        if (i == 5) {
            break;
        }
    }
    return 0;
}
