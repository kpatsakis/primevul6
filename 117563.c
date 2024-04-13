md_leave_child_containers(MD_CTX* ctx, int n_keep)
{
    int ret = 0;

    while(ctx->n_containers > n_keep) {
        MD_CONTAINER* c = &ctx->containers[ctx->n_containers-1];
        int is_ordered_list = FALSE;

        switch(c->ch) {
            case _T(')'):
            case _T('.'):
                is_ordered_list = TRUE;
                MD_FALLTHROUGH();

            case _T('-'):
            case _T('+'):
            case _T('*'):
                MD_CHECK(md_push_container_bytes(ctx, MD_BLOCK_LI,
                                c->task_mark_off, (c->is_task ? CH(c->task_mark_off) : 0),
                                MD_BLOCK_CONTAINER_CLOSER));
                MD_CHECK(md_push_container_bytes(ctx,
                                (is_ordered_list ? MD_BLOCK_OL : MD_BLOCK_UL), 0,
                                c->ch, MD_BLOCK_CONTAINER_CLOSER));
                break;

            case _T('>'):
                MD_CHECK(md_push_container_bytes(ctx, MD_BLOCK_QUOTE, 0,
                                0, MD_BLOCK_CONTAINER_CLOSER));
                break;

            default:
                MD_UNREACHABLE();
                break;
        }

        ctx->n_containers--;
    }

abort:
    return ret;
}