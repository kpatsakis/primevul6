md_scan_for_html_closer(MD_CTX* ctx, const MD_CHAR* str, MD_SIZE len,
                        const MD_LINE* lines, int n_lines,
                        OFF beg, OFF max_end, OFF* p_end,
                        OFF* p_scan_horizon)
{
    OFF off = beg;
    int i = 0;

    if(off < *p_scan_horizon  &&  *p_scan_horizon >= max_end - len) {
        /* We have already scanned the range up to the max_end so we know
         * there is nothing to see. */
        return FALSE;
    }

    while(TRUE) {
        while(off + len <= lines[i].end  &&  off + len <= max_end) {
            if(md_ascii_eq(STR(off), str, len)) {
                /* Success. */
                *p_end = off + len;
                return TRUE;
            }
            off++;
        }

        i++;
        if(off >= max_end  ||  i >= n_lines) {
            /* Failure. */
            *p_scan_horizon = off;
            return FALSE;
        }

        off = lines[i].beg;
    }
}