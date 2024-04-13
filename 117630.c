    md_decode_utf16le__(const CHAR* str, SZ str_size, SZ* p_size)
    {
        if(IS_UTF16_SURROGATE_HI(str[0])) {
            if(1 < str_size && IS_UTF16_SURROGATE_LO(str[1])) {
                if(p_size != NULL)
                    *p_size = 2;
                return UTF16_DECODE_SURROGATE(str[0], str[1]);
            }
        }

        if(p_size != NULL)
            *p_size = 1;
        return str[0];
    }