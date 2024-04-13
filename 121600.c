size_t local_atoi(const char *data) {
    const char * odata = data;
    size_t retval = 0;
    int counter = 0;
    while (*data) {
        if (*data >= '0' && *data <='9') {
            retval *= 10;
            retval += *data - '0';
            ++data;
            ++counter;
            if (counter > 16) {
                fprintf(stderr, "Could not allocate so much memory %s\n", odata);
                exit(1);
            }
        } else if ('M' == *data) {
            retval *= 1024 * 1024;
            break;
        } else if ('K' == *data) {
            retval *= 1024;
            break;
        } else {
            fprintf(stderr, "Could not allocate alphanumeric memory %s\n", odata);
            exit(1);
        }
    }
    return retval;
}