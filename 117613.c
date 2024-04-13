    md_decode_unicode(const CHAR* str, OFF off, SZ str_size, SZ* p_size)
    {
        *p_size = 1;
        return (unsigned) str[off];
    }