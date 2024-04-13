md_is_html_processing_instruction(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg;

    if(off + 2 >= lines[0].end)
        return FALSE;
    if(CH(off+1) != _T('?'))
        return FALSE;
    off += 2;

    return md_scan_for_html_closer(ctx, _T("?>"), 2,
                lines, n_lines, off, max_end, p_end, &ctx->html_proc_instr_horizon);
}