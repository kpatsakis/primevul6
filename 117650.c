md_is_link_label(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg,
                 OFF* p_end, int* p_beg_line_index, int* p_end_line_index,
                 OFF* p_contents_beg, OFF* p_contents_end)
{
    OFF off = beg;
    OFF contents_beg = 0;
    OFF contents_end = 0;
    int line_index = 0;
    int len = 0;

    if(CH(off) != _T('['))
        return FALSE;
    off++;

    while(1) {
        OFF line_end = lines[line_index].end;

        while(off < line_end) {
            if(CH(off) == _T('\\')  &&  off+1 < ctx->size  &&  (ISPUNCT(off+1) || ISNEWLINE(off+1))) {
                if(contents_end == 0) {
                    contents_beg = off;
                    *p_beg_line_index = line_index;
                }
                contents_end = off + 2;
                off += 2;
            } else if(CH(off) == _T('[')) {
                return FALSE;
            } else if(CH(off) == _T(']')) {
                if(contents_beg < contents_end) {
                    /* Success. */
                    *p_contents_beg = contents_beg;
                    *p_contents_end = contents_end;
                    *p_end = off+1;
                    *p_end_line_index = line_index;
                    return TRUE;
                } else {
                    /* Link label must have some non-whitespace contents. */
                    return FALSE;
                }
            } else {
                unsigned codepoint;
                SZ char_size;

                codepoint = md_decode_unicode(ctx->text, off, ctx->size, &char_size);
                if(!ISUNICODEWHITESPACE_(codepoint)) {
                    if(contents_end == 0) {
                        contents_beg = off;
                        *p_beg_line_index = line_index;
                    }
                    contents_end = off + char_size;
                }

                off += char_size;
            }

            len++;
            if(len > 999)
                return FALSE;
        }

        line_index++;
        len++;
        if(line_index < n_lines)
            off = lines[line_index].beg;
        else
            break;
    }

    return FALSE;
}