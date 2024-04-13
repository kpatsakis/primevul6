bool starts_with(const char * a, const char * b) {
    while (*b) {
        if (*a != *b) {
            return false;
        }
        ++a;
        ++b;
    }
    return true;
}