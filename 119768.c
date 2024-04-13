static inline int mystrcmp(char **p, const char *sample)
{
    int len = strlen(sample);
    if (strncmp(*p, sample, len) == 0) {
        (*p) += len;
        return 1;
    } else
        return 0;
}