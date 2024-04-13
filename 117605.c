md_is_setext_underline(MD_CTX* ctx, OFF beg, OFF* p_end, unsigned* p_level)
{
    OFF off = beg + 1;

    while(off < ctx->size  &&  CH(off) == CH(beg))
        off++;

    /* Optionally, space(s) can follow. */
    while(off < ctx->size  &&  CH(off) == _T(' '))
        off++;

    /* But nothing more is allowed on the line. */
    if(off < ctx->size  &&  !ISNEWLINE(off))
        return FALSE;

    *p_level = (CH(beg) == _T('=') ? 1 : 2);
    *p_end = off;
    return TRUE;
}