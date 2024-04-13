md_is_autolink_uri(MD_CTX* ctx, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg+1;

    MD_ASSERT(CH(beg) == _T('<'));

    /* Check for scheme. */
    if(off >= max_end  ||  !ISASCII(off))
        return FALSE;
    off++;
    while(1) {
        if(off >= max_end)
            return FALSE;
        if(off - beg > 32)
            return FALSE;
        if(CH(off) == _T(':')  &&  off - beg >= 3)
            break;
        if(!ISALNUM(off) && CH(off) != _T('+') && CH(off) != _T('-') && CH(off) != _T('.'))
            return FALSE;
        off++;
    }

    /* Check the path after the scheme. */
    while(off < max_end  &&  CH(off) != _T('>')) {
        if(ISWHITESPACE(off) || ISCNTRL(off) || CH(off) == _T('<'))
            return FALSE;
        off++;
    }

    if(off >= max_end)
        return FALSE;

    MD_ASSERT(CH(off) == _T('>'));
    *p_end = off+1;
    return TRUE;
}