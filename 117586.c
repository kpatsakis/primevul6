md_is_table_underline(MD_CTX* ctx, OFF beg, OFF* p_end, unsigned* p_col_count)
{
    OFF off = beg;
    int found_pipe = FALSE;
    unsigned col_count = 0;

    if(off < ctx->size  &&  CH(off) == _T('|')) {
        found_pipe = TRUE;
        off++;
        while(off < ctx->size  &&  ISWHITESPACE(off))
            off++;
    }

    while(1) {
        OFF cell_beg;
        int delimited = FALSE;

        /* Cell underline ("-----", ":----", "----:" or ":----:") */
        cell_beg = off;
        if(off < ctx->size  &&  CH(off) == _T(':'))
            off++;
        while(off < ctx->size  &&  CH(off) == _T('-'))
            off++;
        if(off < ctx->size  &&  CH(off) == _T(':'))
            off++;
        if(off - cell_beg < 3)
            return FALSE;

        col_count++;

        /* Pipe delimiter (optional at the end of line). */
        while(off < ctx->size  &&  ISWHITESPACE(off))
            off++;
        if(off < ctx->size  &&  CH(off) == _T('|')) {
            delimited = TRUE;
            found_pipe =  TRUE;
            off++;
            while(off < ctx->size  &&  ISWHITESPACE(off))
                off++;
        }

        /* Success, if we reach end of line. */
        if(off >= ctx->size  ||  ISNEWLINE(off))
            break;

        if(!delimited)
            return FALSE;
    }

    if(!found_pipe)
        return FALSE;

    *p_end = off;
    *p_col_count = col_count;
    return TRUE;
}