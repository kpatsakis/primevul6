md_enter_child_containers(MD_CTX* ctx, int n_children)
{
    int i;
    int ret = 0;

    for(i = ctx->n_containers - n_children; i < ctx->n_containers; i++) {
        MD_CONTAINER* c = &ctx->containers[i];
        int is_ordered_list = FALSE;

        switch(c->ch) {
            case _T(')'):
            case _T('.'):
                is_ordered_list = TRUE;
                MD_FALLTHROUGH();

            case _T('-'):
            case _T('+'):
            case _T('*'):
                /* Remember offset in ctx->block_bytes so we can revisit the
                 * block if we detect it is a loose list. */
                md_end_current_block(ctx);
                c->block_byte_off = ctx->n_block_bytes;

                MD_CHECK(md_push_container_bytes(ctx,
                                (is_ordered_list ? MD_BLOCK_OL : MD_BLOCK_UL),
                                c->start, c->ch, MD_BLOCK_CONTAINER_OPENER));
                MD_CHECK(md_push_container_bytes(ctx, MD_BLOCK_LI,
                                c->task_mark_off,
                                (c->is_task ? CH(c->task_mark_off) : 0),
                                MD_BLOCK_CONTAINER_OPENER));
                break;

            case _T('>'):
                MD_CHECK(md_push_container_bytes(ctx, MD_BLOCK_QUOTE, 0, 0, MD_BLOCK_CONTAINER_OPENER));
                break;

            default:
                MD_UNREACHABLE();
                break;
        }
    }

abort:
    return ret;
}