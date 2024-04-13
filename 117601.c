md_is_code_span(MD_CTX* ctx, const MD_LINE* lines, int n_lines, OFF beg,
                OFF* p_opener_beg, OFF* p_opener_end,
                OFF* p_closer_beg, OFF* p_closer_end,
                OFF last_potential_closers[CODESPAN_MARK_MAXLEN],
                int* p_reached_paragraph_end)
{
    OFF opener_beg = beg;
    OFF opener_end;
    OFF closer_beg;
    OFF closer_end;
    SZ mark_len;
    OFF line_end;
    int has_space_after_opener = FALSE;
    int has_eol_after_opener = FALSE;
    int has_space_before_closer = FALSE;
    int has_eol_before_closer = FALSE;
    int has_only_space = TRUE;
    int line_index = 0;

    line_end = lines[0].end;
    opener_end = opener_beg;
    while(opener_end < line_end  &&  CH(opener_end) == _T('`'))
        opener_end++;
    has_space_after_opener = (opener_end < line_end && CH(opener_end) == _T(' '));
    has_eol_after_opener = (opener_end == line_end);

    /* The caller needs to know end of the opening mark even if we fail. */
    *p_opener_end = opener_end;

    mark_len = opener_end - opener_beg;
    if(mark_len > CODESPAN_MARK_MAXLEN)
        return FALSE;

    /* Check whether we already know there is no closer of this length.
     * If so, re-scan does no sense. This fixes issue #59. */
    if(last_potential_closers[mark_len-1] >= lines[n_lines-1].end  ||
       (*p_reached_paragraph_end  &&  last_potential_closers[mark_len-1] < opener_end))
        return FALSE;

    closer_beg = opener_end;
    closer_end = opener_end;

    /* Find closer mark. */
    while(TRUE) {
        while(closer_beg < line_end  &&  CH(closer_beg) != _T('`')) {
            if(CH(closer_beg) != _T(' '))
                has_only_space = FALSE;
            closer_beg++;
        }
        closer_end = closer_beg;
        while(closer_end < line_end  &&  CH(closer_end) == _T('`'))
            closer_end++;

        if(closer_end - closer_beg == mark_len) {
            /* Success. */
            has_space_before_closer = (closer_beg > lines[line_index].beg && CH(closer_beg-1) == _T(' '));
            has_eol_before_closer = (closer_beg == lines[line_index].beg);
            break;
        }

        if(closer_end - closer_beg > 0) {
            /* We have found a back-tick which is not part of the closer. */
            has_only_space = FALSE;

            /* But if we eventually fail, remember it as a potential closer
             * of its own length for future attempts. This mitigates needs for
             * rescans. */
            if(closer_end - closer_beg < CODESPAN_MARK_MAXLEN) {
                if(closer_beg > last_potential_closers[closer_end - closer_beg - 1])
                    last_potential_closers[closer_end - closer_beg - 1] = closer_beg;
            }
        }

        if(closer_end >= line_end) {
            line_index++;
            if(line_index >= n_lines) {
                /* Reached end of the paragraph and still nothing. */
                *p_reached_paragraph_end = TRUE;
                return FALSE;
            }
            /* Try on the next line. */
            line_end = lines[line_index].end;
            closer_beg = lines[line_index].beg;
        } else {
            closer_beg = closer_end;
        }
    }

    /* If there is a space or a new line both after and before the opener
     * (and if the code span is not made of spaces only), consume one initial
     * and one trailing space as part of the marks. */
    if(!has_only_space  &&
       (has_space_after_opener || has_eol_after_opener)  &&
       (has_space_before_closer || has_eol_before_closer))
    {
        if(has_space_after_opener)
            opener_end++;
        else
            opener_end = lines[1].beg;

        if(has_space_before_closer)
            closer_beg--;
        else {
            closer_beg = lines[line_index-1].end;
            /* We need to eat the preceding "\r\n" but not any line trailing
             * spaces. */
            while(closer_beg < ctx->size  &&  ISBLANK(closer_beg))
                closer_beg++;
        }
    }

    *p_opener_beg = opener_beg;
    *p_opener_end = opener_end;
    *p_closer_beg = closer_beg;
    *p_closer_end = closer_end;
    return TRUE;
}