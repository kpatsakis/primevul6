md_is_html_comment(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;

    MD_ASSERT(CH(beg) == _T('<'));

    if(off + 4 >= lines[0].end)
        return FALSE;
    if(CH(off+1) != _T('!')  ||  CH(off+2) != _T('-')  ||  CH(off+3) != _T('-'))
        return FALSE;
    off += 4;

    /* ">" and "->" must not follow the opening. */
    if(off < lines[0].end  &&  CH(off) == _T('>'))
        return FALSE;
    if(off+1 < lines[0].end  &&  CH(off) == _T('-')  &&  CH(off+1) == _T('>'))
        return FALSE;

    /* HTML comment must not contain "--", so we scan just for "--" instead
     * of "-->" and verify manually that '>' follows. */
    if(md_scan_for_html_closer(ctx, _T("--"), 2,
                lines, n_lines, off, max_end, p_end, &ctx->html_comment_horizon))
    {
        if(*p_end < max_end  &&  CH(*p_end) == _T('>')) {
            *p_end = *p_end + 1;
            return TRUE;
        }
    }

    return FALSE;
}