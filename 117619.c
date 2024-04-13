md_is_html_declaration(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;

    if(off + 2 >= lines[0].end)
        return FALSE;
    if(CH(off+1) != _T('!'))
        return FALSE;
    off += 2;

    /* Declaration name. */
    if(off >= lines[0].end  ||  !ISALPHA(off))
        return FALSE;
    off++;
    while(off < lines[0].end  &&  ISALPHA(off))
        off++;
    if(off < lines[0].end  &&  !ISWHITESPACE(off))
        return FALSE;

    return md_scan_for_html_closer(ctx, _T(">"), 1,
                lines, n_lines, off, max_end, p_end, &ctx->html_decl_horizon);
}