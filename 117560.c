md_merge_lines_alloc(MD_CTX* ctx, OFF beg, OFF end, const MD_LINE* lines, int n_lines,
                    CHAR line_break_replacement_char, CHAR** p_str, SZ* p_size)
{
    CHAR* buffer;

    buffer = (CHAR*) malloc(sizeof(CHAR) * (end - beg));
    if(buffer == NULL) {
        MD_LOG("malloc() failed.");
        return -1;
    }

    md_merge_lines(ctx, beg, end, lines, n_lines,
                line_break_replacement_char, buffer, p_size);

    *p_str = buffer;
    return 0;
}