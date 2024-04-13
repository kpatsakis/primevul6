md_is_hr_line(MD_CTX* ctx, OFF beg, OFF* p_end, OFF* p_killer)
{
    OFF off = beg + 1;
    int n = 1;

    while(off < ctx->size  &&  (CH(off) == CH(beg) || CH(off) == _T(' ') || CH(off) == _T('\t'))) {
        if(CH(off) == CH(beg))
            n++;
        off++;
    }

    if(n < 3) {
        *p_killer = off;
        return FALSE;
    }

    /* Nothing else can be present on the line. */
    if(off < ctx->size  &&  !ISNEWLINE(off)) {
        *p_killer = off;
        return FALSE;
    }

    *p_end = off;
    return TRUE;
}