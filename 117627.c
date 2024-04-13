    md_decode_utf8_before__(MD_CTX* ctx, OFF off)
    {
        if(!IS_UTF8_LEAD1(CH(off-1))) {
            if(off > 1 && IS_UTF8_LEAD2(CH(off-2)) && IS_UTF8_TAIL(CH(off-1)))
                return (((unsigned int)CH(off-2) & 0x1f) << 6) |
                       (((unsigned int)CH(off-1) & 0x3f) << 0);

            if(off > 2 && IS_UTF8_LEAD3(CH(off-3)) && IS_UTF8_TAIL(CH(off-2)) && IS_UTF8_TAIL(CH(off-1)))
                return (((unsigned int)CH(off-3) & 0x0f) << 12) |
                       (((unsigned int)CH(off-2) & 0x3f) << 6) |
                       (((unsigned int)CH(off-1) & 0x3f) << 0);

            if(off > 3 && IS_UTF8_LEAD4(CH(off-4)) && IS_UTF8_TAIL(CH(off-3)) && IS_UTF8_TAIL(CH(off-2)) && IS_UTF8_TAIL(CH(off-1)))
                return (((unsigned int)CH(off-4) & 0x07) << 18) |
                       (((unsigned int)CH(off-3) & 0x3f) << 12) |
                       (((unsigned int)CH(off-2) & 0x3f) << 6) |
                       (((unsigned int)CH(off-1) & 0x3f) << 0);
        }

        return (unsigned) CH(off-1);
    }