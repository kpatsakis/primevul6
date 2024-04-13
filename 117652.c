md_is_dec_entity_contents(MD_CTX* ctx, const CHAR* text, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;
    MD_UNUSED(ctx);

    while(off < max_end  &&  ISDIGIT_(text[off])  &&  off - beg <= 8)
        off++;

    if(1 <= off - beg  &&  off - beg <= 7) {
        *p_end = off;
        return TRUE;
    } else {
        return FALSE;
    }
}