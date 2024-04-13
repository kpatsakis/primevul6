md_is_autolink_email(MD_CTX* ctx, OFF beg, OFF max_end, OFF* p_end)
{
    OFF off = beg + 1;
    int label_len;

    MD_ASSERT(CH(beg) == _T('<'));

    /* The code should correspond to this regexp:
            /^[a-zA-Z0-9.!#$%&'*+\/=?^_`{|}~-]+
            @[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?
            (?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$/
     */

    /* Username (before '@'). */
    while(off < max_end  &&  (ISALNUM(off) || ISANYOF(off, _T(".!#$%&'*+/=?^_`{|}~-"))))
        off++;
    if(off <= beg+1)
        return FALSE;

    /* '@' */
    if(off >= max_end  ||  CH(off) != _T('@'))
        return FALSE;
    off++;

    /* Labels delimited with '.'; each label is sequence of 1 - 63 alnum
     * characters or '-', but '-' is not allowed as first or last char. */
    label_len = 0;
    while(off < max_end) {
        if(ISALNUM(off))
            label_len++;
        else if(CH(off) == _T('-')  &&  label_len > 0)
            label_len++;
        else if(CH(off) == _T('.')  &&  label_len > 0  &&  CH(off-1) != _T('-'))
            label_len = 0;
        else
            break;

        if(label_len > 63)
            return FALSE;

        off++;
    }

    if(label_len <= 0  || off >= max_end  ||  CH(off) != _T('>') ||  CH(off-1) == _T('-'))
        return FALSE;

    *p_end = off+1;
    return TRUE;
}