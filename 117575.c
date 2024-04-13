md_push_block_bytes(MD_CTX* ctx, int n_bytes)
{
    void* ptr;

    if(ctx->n_block_bytes + n_bytes > ctx->alloc_block_bytes) {
        void* new_block_bytes;

        ctx->alloc_block_bytes = (ctx->alloc_block_bytes > 0
                ? ctx->alloc_block_bytes + ctx->alloc_block_bytes / 2
                : 512);
        new_block_bytes = realloc(ctx->block_bytes, ctx->alloc_block_bytes);
        if(new_block_bytes == NULL) {
            MD_LOG("realloc() failed.");
            return NULL;
        }

        /* Fix the ->current_block after the reallocation. */
        if(ctx->current_block != NULL) {
            OFF off_current_block = (char*) ctx->current_block - (char*) ctx->block_bytes;
            ctx->current_block = (MD_BLOCK*) ((char*) new_block_bytes + off_current_block);
        }

        ctx->block_bytes = new_block_bytes;
    }

    ptr = (char*)ctx->block_bytes + ctx->n_block_bytes;
    ctx->n_block_bytes += n_bytes;
    return ptr;
}