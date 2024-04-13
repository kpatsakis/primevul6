md_merge_lines(MD_CTX* ctx, OFF beg, OFF end, const MD_LINE* lines, int n_lines,
               CHAR line_break_replacement_char, CHAR* buffer, SZ* p_size)
{
    CHAR* ptr = buffer;
    int line_index = 0;
    OFF off = beg;

    MD_UNUSED(n_lines);

    while(1) {
        const MD_LINE* line = &lines[line_index];
        OFF line_end = line->end;
        if(end < line_end)
            line_end = end;

        while(off < line_end) {
            *ptr = CH(off);
            ptr++;
            off++;
        }

        if(off >= end) {
            *p_size = ptr - buffer;
            return;
        }

        *ptr = line_break_replacement_char;
        ptr++;

        line_index++;
        off = lines[line_index].beg;
    }
}