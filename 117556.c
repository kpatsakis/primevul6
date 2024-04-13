md_is_link_destination_A(MD_CTX* ctx, OFF beg, OFF max_end, OFF* p_end,
                         OFF* p_contents_beg, OFF* p_contents_end)
{
    OFF off = beg;

    if(off >= max_end  ||  CH(off) != _T('<'))
        return FALSE;
    off++;

    while(off < max_end) {
        if(CH(off) == _T('\\')  &&  off+1 < max_end  &&  ISPUNCT(off+1)) {
            off += 2;
            continue;
        }

        if(ISNEWLINE(off)  ||  CH(off) == _T('<'))
            return FALSE;

        if(CH(off) == _T('>')) {
            /* Success. */
            *p_contents_beg = beg+1;
            *p_contents_end = off;
            *p_end = off+1;
            return TRUE;
        }

        off++;
    }

    return FALSE;
}