static unsigned char *get_params(unsigned char *p, int *param, int *len)
{
    int n;

    *len = 0;
    while (*p != '\0') {
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (isdigit(*p)) {
            for (n = 0; isdigit(*p); p++) {
                n = n * 10 + (*p - '0');
            }
            if (*len < 10) {
                param[(*len)++] = n;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            if (*p == ';') {
                p++;
            }
        } else if (*p == ';') {
            if (*len < 10) {
                param[(*len)++] = 0;
            }
            p++;
        } else
            break;
    }
    return p;
}