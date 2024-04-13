int pure_strcmp(const char * const s1, const char * const s2)
{
    const size_t s1_len = strlen(s1);
    const size_t s2_len = strlen(s2);
    const size_t len = (s1_len < s2_len) ? s1_len : s2_len;

    return pure_memcmp(s1, s2, len + 1);
}