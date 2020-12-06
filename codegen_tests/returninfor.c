
void put_int(int x);

int main() {
    for (int i = 0; i < 10; i += 1) {
        put_int(i);
        return i;
        put_int(i + 1);
    }
    return -1;
}
