md_mark_get_ptr(MD_CTX* ctx, int mark_index)
{
    void* ptr;
    MD_MARK* mark = &ctx->marks[mark_index];
    MD_ASSERT(mark->ch == 'D');
    memcpy(&ptr, mark, sizeof(void*));
    return ptr;
}