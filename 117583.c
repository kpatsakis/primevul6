md_fnv1a(unsigned base, const void* data, size_t n)
{
    const unsigned char* buf = (const unsigned char*) data;
    unsigned hash = base;
    size_t i;

    for(i = 0; i < n; i++) {
        hash ^= buf[i];
        hash *= MD_FNV1A_PRIME;
    }

    return hash;
}