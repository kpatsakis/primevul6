    md_decode_utf8__(const CHAR* str, SZ str_size, SZ* p_size)
    {
        if(!IS_UTF8_LEAD1(str[0])) {
            if(IS_UTF8_LEAD2(str[0])) {
                if(1 < str_size && IS_UTF8_TAIL(str[1])) {
                    if(p_size != NULL)
                        *p_size = 2;

                    return (((unsigned int)str[0] & 0x1f) << 6) |
                           (((unsigned int)str[1] & 0x3f) << 0);
                }
            } else if(IS_UTF8_LEAD3(str[0])) {
                if(2 < str_size && IS_UTF8_TAIL(str[1]) && IS_UTF8_TAIL(str[2])) {
                    if(p_size != NULL)
                        *p_size = 3;

                    return (((unsigned int)str[0] & 0x0f) << 12) |
                           (((unsigned int)str[1] & 0x3f) << 6) |
                           (((unsigned int)str[2] & 0x3f) << 0);
                }
            } else if(IS_UTF8_LEAD4(str[0])) {
                if(3 < str_size && IS_UTF8_TAIL(str[1]) && IS_UTF8_TAIL(str[2]) && IS_UTF8_TAIL(str[3])) {
                    if(p_size != NULL)
                        *p_size = 4;

                    return (((unsigned int)str[0] & 0x07) << 18) |
                           (((unsigned int)str[1] & 0x3f) << 12) |
                           (((unsigned int)str[2] & 0x3f) << 6) |
                           (((unsigned int)str[3] & 0x3f) << 0);
                }
            }
        }

        if(p_size != NULL)
            *p_size = 1;
        return (unsigned) str[0];
    }