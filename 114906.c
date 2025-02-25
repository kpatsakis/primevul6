static int scantag(const char* in, char* buffer, int* lenp) {
    int len;

    for (len = 0; len < 128; len++) {
        const char c = *in++;
        switch (c) {
        case '\0':
            return 0;
        case '<':
            return 0;
        case '>':
            buffer[len] = '\0';
            *lenp = len+1;
            return 1;
        default:
            break;
        }
        buffer[len] = c;
    }
    return 0;
}