int fun(int i) {
    return i + i;
}

int main () {
    void (*func_ptr)(int) = &fun;
    (*func_ptr)(10);

    int c;
    for (c = 0; c < 3; c++) {
        c = c + 1;
    }
    return c;
}