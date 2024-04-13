    md_decode_utf16le_before__(MD_CTX* ctx, OFF off)
    {
        if(off > 2 && IS_UTF16_SURROGATE_HI(CH(off-2)) && IS_UTF16_SURROGATE_LO(CH(off-1)))
            return UTF16_DECODE_SURROGATE(CH(off-2), CH(off-1));

        return CH(off);
    }