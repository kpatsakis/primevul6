md_process_code_block_contents(MD_CTX* ctx, int is_fenced, const MD_VERBATIMLINE* lines, int n_lines)
{
    if(is_fenced) {
        /* Skip the first line in case of fenced code: It is the fence.
         * (Only the starting fence is present due to logic in md_analyze_line().) */
        lines++;
        n_lines--;
    } else {
        /* Ignore blank lines at start/end of indented code block. */
        while(n_lines > 0  &&  lines[0].beg == lines[0].end) {
            lines++;
            n_lines--;
        }
        while(n_lines > 0  &&  lines[n_lines-1].beg == lines[n_lines-1].end) {
            n_lines--;
        }
    }

    if(n_lines == 0)
        return 0;

    return md_process_verbatim_block_contents(ctx, MD_TEXT_CODE, lines, n_lines);
}