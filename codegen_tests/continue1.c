
void put_int(int x);

int main() {
    for (int i = 0; i < 10; i += 1) {
        if (i == 3 || i == 7) {
            continue;
        }
        put_int(i);
    }
    return 0;
}
