md_push_container(MD_CTX* ctx, const MD_CONTAINER* container)
{
    if(ctx->n_containers >= ctx->alloc_containers) {
        MD_CONTAINER* new_containers;

        ctx->alloc_containers = (ctx->alloc_containers > 0
                ? ctx->alloc_containers + ctx->alloc_containers / 2
                : 16);
        new_containers = realloc(ctx->containers, ctx->alloc_containers * sizeof(MD_CONTAINER));
        if(new_containers == NULL) {
            MD_LOG("realloc() failed.");
            return -1;
        }

        ctx->containers = new_containers;
    }

    memcpy(&ctx->containers[ctx->n_containers++], container, sizeof(MD_CONTAINER));
    return 0;
}