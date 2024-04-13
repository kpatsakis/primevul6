static void *ok_stdlib_alloc(void *user_data, size_t size) {
    (void)user_data;
    return malloc(size);
}