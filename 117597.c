md_mark_store_ptr(MD_CTX* ctx, int mark_index, void* ptr)
{
    MD_MARK* mark = &ctx->marks[mark_index];
    MD_ASSERT(mark->ch == 'D');

    /* Check only members beg and end are misused for this. */
    MD_ASSERT(sizeof(void*) <= 2 * sizeof(OFF));
    memcpy(mark, &ptr, sizeof(void*));
}