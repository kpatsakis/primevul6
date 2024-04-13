md_is_html_block_end_condition(MD_CTX* ctx, OFF beg, OFF* p_end)
{
    switch(ctx->html_block_type) {
        case 1:
        {
            OFF off = beg;

            while(off < ctx->size  &&  !ISNEWLINE(off)) {
                if(CH(off) == _T('<')) {
                    if(md_ascii_case_eq(STR(off), _T("</script>"), 9)) {
                        *p_end = off + 9;
                        return TRUE;
                    }

                    if(md_ascii_case_eq(STR(off), _T("</style>"), 8)) {
                        *p_end = off + 8;
                        return TRUE;
                    }

                    if(md_ascii_case_eq(STR(off), _T("</pre>"), 6)) {
                        *p_end = off + 6;
                        return TRUE;
                    }
                }

                off++;
            }
            *p_end = off;
            return FALSE;
        }

        case 2:
            return (md_line_contains(ctx, beg, _T("-->"), 3, p_end) ? 2 : FALSE);

        case 3:
            return (md_line_contains(ctx, beg, _T("?>"), 2, p_end) ? 3 : FALSE);

        case 4:
            return (md_line_contains(ctx, beg, _T(">"), 1, p_end) ? 4 : FALSE);

        case 5:
            return (md_line_contains(ctx, beg, _T("]]>"), 3, p_end) ? 5 : FALSE);

        case 6:     /* Pass through */
        case 7:
            *p_end = beg;
            return (ISNEWLINE(beg) ? ctx->html_block_type : FALSE);

        default:
            MD_UNREACHABLE();
    }
    return FALSE;
}