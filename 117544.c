md_is_html_tag(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg, OFF max_end, OFF* p_end)
{
    int attr_state;
    OFF off = beg;
    OFF line_end = (n_lines > 0) ? lines[0].end : ctx->size;
    int i = 0;

    MD_ASSERT(CH(beg) == _T('<'));

    if(off + 1 >= line_end)
        return FALSE;
    off++;

    /* For parsing attributes, we need a little state automaton below.
     * State -1: no attributes are allowed.
     * State 0: attribute could follow after some whitespace.
     * State 1: after a whitespace (attribute name may follow).
     * State 2: after attribute name ('=' MAY follow).
     * State 3: after '=' (value specification MUST follow).
     * State 41: in middle of unquoted attribute value.
     * State 42: in middle of single-quoted attribute value.
     * State 43: in middle of double-quoted attribute value.
     */
    attr_state = 0;

    if(CH(off) == _T('/')) {
        /* Closer tag "</ ... >". No attributes may be present. */
        attr_state = -1;
        off++;
    }

    /* Tag name */
    if(off >= line_end  ||  !ISALPHA(off))
        return FALSE;
    off++;
    while(off < line_end  &&  (ISALNUM(off)  ||  CH(off) == _T('-')))
        off++;

    /* (Optional) attributes (if not closer), (optional) '/' (if not closer)
     * and final '>'. */
    while(1) {
        while(off < line_end  &&  !ISNEWLINE(off)) {
            if(attr_state > 40) {
                if(attr_state == 41 && (ISBLANK(off) || ISANYOF(off, _T("\"'=<>`")))) {
                    attr_state = 0;
                    off--;  /* Put the char back for re-inspection in the new state. */
                } else if(attr_state == 42 && CH(off) == _T('\'')) {
                    attr_state = 0;
                } else if(attr_state == 43 && CH(off) == _T('"')) {
                    attr_state = 0;
                }
                off++;
            } else if(ISWHITESPACE(off)) {
                if(attr_state == 0)
                    attr_state = 1;
                off++;
            } else if(attr_state <= 2 && CH(off) == _T('>')) {
                /* End. */
                goto done;
            } else if(attr_state <= 2 && CH(off) == _T('/') && off+1 < line_end && CH(off+1) == _T('>')) {
                /* End with digraph '/>' */
                off++;
                goto done;
            } else if((attr_state == 1 || attr_state == 2) && (ISALPHA(off) || CH(off) == _T('_') || CH(off) == _T(':'))) {
                off++;
                /* Attribute name */
                while(off < line_end && (ISALNUM(off) || ISANYOF(off, _T("_.:-"))))
                    off++;
                attr_state = 2;
            } else if(attr_state == 2 && CH(off) == _T('=')) {
                /* Attribute assignment sign */
                off++;
                attr_state = 3;
            } else if(attr_state == 3) {
                /* Expecting start of attribute value. */
                if(CH(off) == _T('"'))
                    attr_state = 43;
                else if(CH(off) == _T('\''))
                    attr_state = 42;
                else if(!ISANYOF(off, _T("\"'=<>`"))  &&  !ISNEWLINE(off))
                    attr_state = 41;
                else
                    return FALSE;
                off++;
            } else {
                /* Anything unexpected. */
                return FALSE;
            }
        }

        /* We have to be on a single line. See definition of start condition
         * of HTML block, type 7. */
        if(n_lines == 0)
            return FALSE;

        i++;
        if(i >= n_lines)
            return FALSE;

        off = lines[i].beg;
        line_end = lines[i].end;

        if(attr_state == 0  ||  attr_state == 41)
            attr_state = 1;

        if(off >= max_end)
            return FALSE;
    }

done:
    if(off >= max_end)
        return FALSE;

    *p_end = off+1;
    return TRUE;
}