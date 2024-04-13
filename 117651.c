md_is_html_cdata(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    static const CHAR open_str[] = _T("<![CDATA[");
    static const SZ open_size = SIZEOF_ARRAY(open_str) - 1;

    OFF off = beg;

    if(off + open_size >= lines[0].end)
        return FALSE;
    if(memcmp(STR(off), open_str, open_size) != 0)
        return FALSE;
    off += open_size;

    if(lines[n_lines-1].end < max_end)
        max_end = lines[n_lines-1].end - 2;

    return md_scan_for_html_closer(ctx, _T("]]>"), 3,
                lines, n_lines, off, max_end, p_end, &ctx->html_cdata_horizon);
}