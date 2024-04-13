md_is_opening_code_fence(MD_CTX* ctx, OFF beg, OFF* p_end)
{
    OFF off = beg;

    while(off < ctx->size && CH(off) == CH(beg))
        off++;

    /* Fence must have at least three characters. */
    if(off - beg < 3)
        return FALSE;

    ctx->code_fence_length = off - beg;

    /* Optionally, space(s) can follow. */
    while(off < ctx->size  &&  CH(off) == _T(' '))
        off++;

    /* Optionally, an info string can follow. */
    while(off < ctx->size  &&  !ISNEWLINE(off)) {
        /* Backtick-based fence must not contain '`' in the info string. */
        if(CH(beg) == _T('`')  &&  CH(off) == _T('`'))
            return FALSE;
        off++;
    }

    *p_end = off;
    return TRUE;
}