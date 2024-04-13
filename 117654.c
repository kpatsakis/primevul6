md_is_named_entity_contents(MD_CTX* ctx, const CHAR* text, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;
    MD_UNUSED(ctx);

    if(off < max_end  &&  ISALPHA_(text[off]))
        off++;
    else
        return FALSE;

    while(off < max_end  &&  ISALNUM_(text[off])  &&  off - beg <= 48)
        off++;

    if(2 <= off - beg  &&  off - beg <= 48) {
        *p_end = off;
        return TRUE;
    } else {
        return FALSE;
    }
}