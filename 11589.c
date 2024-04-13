cmp_format(const void *p, const void *q)
{
    const char *a = *(char *const *) p;
    const char *b = *(char *const *) q;
    return strcmp(a, b);
}