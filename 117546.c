md_split_emph_mark(MD_CTX* ctx, int mark_index, SZ n)
{
    MD_MARK* mark = &ctx->marks[mark_index];
    int new_mark_index = mark_index + (mark->end - mark->beg - n);
    MD_MARK* dummy = &ctx->marks[new_mark_index];

    MD_ASSERT(mark->end - mark->beg > n);
    MD_ASSERT(dummy->ch == 'D');

    memcpy(dummy, mark, sizeof(MD_MARK));
    mark->end -= n;
    dummy->beg = mark->end;

    return new_mark_index;
}