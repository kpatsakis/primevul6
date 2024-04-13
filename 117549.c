md_mark_chain(MD_CTX* ctx, int mark_index)
{
    MD_MARK* mark = &ctx->marks[mark_index];

    switch(mark->ch) {
        case _T('*'):   return md_asterisk_chain(ctx, mark->flags);
        case _T('_'):   return &UNDERSCORE_OPENERS;
        case _T('~'):   return (mark->end - mark->beg == 1) ? &TILDE_OPENERS_1 : &TILDE_OPENERS_2;
        case _T('['):   return &BRACKET_OPENERS;
        case _T('|'):   return &TABLECELLBOUNDARIES;
        default:        return NULL;
    }
}