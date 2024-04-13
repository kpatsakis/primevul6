md_push_mark(MD_CTX* ctx)
{
    if(ctx->n_marks >= ctx->alloc_marks) {
        MD_MARK* new_marks;

        ctx->alloc_marks = (ctx->alloc_marks > 0
                ? ctx->alloc_marks + ctx->alloc_marks / 2
                : 64);
        new_marks = realloc(ctx->marks, ctx->alloc_marks * sizeof(MD_MARK));
        if(new_marks == NULL) {
            MD_LOG("realloc() failed.");
            return NULL;
        }

        ctx->marks = new_marks;
    }

    return &ctx->marks[ctx->n_marks++];
}