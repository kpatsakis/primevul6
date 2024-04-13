md_is_hex_entity_contents(MD_CTX* ctx, const CHAR* text, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;
    MD_UNUSED(ctx);

    while(off < max_end  &&  ISXDIGIT_(text[off])  &&  off - beg <= 8)
        off++;

    if(1 <= off - beg  &&  off - beg <= 6) {
        *p_end = off;
        return TRUE;
    } else {
        return FALSE;
    }
}