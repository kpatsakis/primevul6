md_is_html_any(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    MD_ASSERT(CH(beg) == _T('<'));
    return (md_is_html_tag(ctx, lines, n_lines, beg, max_end, p_end)  ||
            md_is_html_comment(ctx, lines, n_lines, beg, max_end, p_end)  ||
            md_is_html_processing_instruction(ctx, lines, n_lines, beg, max_end, p_end)  ||
            md_is_html_declaration(ctx, lines, n_lines, beg, max_end, p_end)  ||
            md_is_html_cdata(ctx, lines, n_lines, beg, max_end, p_end));
}