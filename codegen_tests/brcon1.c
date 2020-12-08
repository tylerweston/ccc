
void put_int(int x);
void putascii(int y);
void putvar(int vchar, int vval) {
    putascii(vchar);
    putascii(58);
    putascii(32);
    putascii(48 + vval);
    putascii(10);
}

int main() {
    for (int i = 0; i < 10; i += 1) {
        int j = -1;
        putvar(105, i);
        if (i == 9) { break; }
        if (i == 6) { continue; }
        while (j < 10) {
            j += 1;
            if (j == 8) { break; }
            if (j == 5) {
                j += 1;
                continue;
            }
            putvar(106, j);
        }
    }
    return 0;
}

