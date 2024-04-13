md_analyze_permissive_email_autolink(MD_CTX* ctx, int mark_index)
{
    MD_MARK* opener = &ctx->marks[mark_index];
    int closer_index;
    MD_MARK* closer;
    OFF beg = opener->beg;
    OFF end = opener->end;
    int dot_count = 0;

    MD_ASSERT(CH(beg) == _T('@'));

    /* Scan for name before '@'. */
    while(beg > 0  &&  (ISALNUM(beg-1) || ISANYOF(beg-1, _T(".-_+"))))
        beg--;

    /* Scan for domain after '@'. */
    while(end < ctx->size  &&  (ISALNUM(end) || ISANYOF(end, _T(".-_")))) {
        if(CH(end) == _T('.'))
            dot_count++;
        end++;
    }
    if(CH(end-1) == _T('.')) {  /* Final '.' not part of it. */
        dot_count--;
        end--;
    }
    else if(ISANYOF2(end-1, _T('-'), _T('_'))) /* These are forbidden at the end. */
        return;
    if(CH(end-1) == _T('@')  ||  dot_count == 0)
        return;

    /* Ok. Lets call it auto-link. Adapt opener and create closer to zero
     * length so all the contents becomes the link text. */
    closer_index = mark_index + 1;
    closer = &ctx->marks[closer_index];
    MD_ASSERT(closer->ch == 'D');

    opener->beg = beg;
    opener->end = beg;
    closer->ch = opener->ch;
    closer->beg = end;
    closer->end = end;
    md_resolve_range(ctx, NULL, mark_index, closer_index);
}