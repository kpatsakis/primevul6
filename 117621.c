md_is_entity_str(MD_CTX* ctx, const CHAR* text, OFF beg, OFF max_end, OFF* p_end)
{
    int is_contents;
    OFF off = beg;

    MD_ASSERT(text[off] == _T('&'));
    off++;

    if(off+2 < max_end  &&  text[off] == _T('#')  &&  (text[off+1] == _T('x') || text[off+1] == _T('X')))
        is_contents = md_is_hex_entity_contents(ctx, text, off+2, max_end, &off);
    else if(off+1 < max_end  &&  text[off] == _T('#'))
        is_contents = md_is_dec_entity_contents(ctx, text, off+1, max_end, &off);
    else
        is_contents = md_is_named_entity_contents(ctx, text, off, max_end, &off);

    if(is_contents  &&  off < max_end  &&  text[off] == _T(';')) {
        *p_end = off+1;
        return TRUE;
    } else {
        return FALSE;
    }
}