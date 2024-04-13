md_rollback(MD_CTX* ctx, int opener_index, int closer_index, int how)
{
    int i;
    int mark_index;

    /* Cut all unresolved openers at the mark index. */
    for(i = OPENERS_CHAIN_FIRST; i < OPENERS_CHAIN_LAST+1; i++) {
        MD_MARKCHAIN* chain = &ctx->mark_chains[i];

        while(chain->tail >= opener_index)
            chain->tail = ctx->marks[chain->tail].prev;

        if(chain->tail >= 0)
            ctx->marks[chain->tail].next = -1;
        else
            chain->head = -1;
    }

    /* Go backwards so that unresolved openers are re-added into their
     * respective chains, in the right order. */
    mark_index = closer_index - 1;
    while(mark_index > opener_index) {
        MD_MARK* mark = &ctx->marks[mark_index];
        int mark_flags = mark->flags;
        int discard_flag = (how == MD_ROLLBACK_ALL);

        if(mark->flags & MD_MARK_CLOSER) {
            int mark_opener_index = mark->prev;

            /* Undo opener BEFORE the range. */
            if(mark_opener_index < opener_index) {
                MD_MARK* mark_opener = &ctx->marks[mark_opener_index];
                MD_MARKCHAIN* chain;

                mark_opener->flags &= ~(MD_MARK_OPENER | MD_MARK_CLOSER | MD_MARK_RESOLVED);
                chain = md_mark_chain(ctx, opener_index);
                if(chain != NULL) {
                    md_mark_chain_append(ctx, chain, mark_opener_index);
                    discard_flag = 1;
                }
            }
        }

        /* And reset our flags. */
        if(discard_flag)
            mark->flags &= ~(MD_MARK_OPENER | MD_MARK_CLOSER | MD_MARK_RESOLVED);

        /* Jump as far as we can over unresolved or non-interesting marks. */
        switch(how) {
            case MD_ROLLBACK_CROSSING:
                if((mark_flags & MD_MARK_CLOSER)  &&  mark->prev > opener_index) {
                    /* If we are closer with opener INSIDE the range, there may
                     * not be any other crosser inside the subrange. */
                    mark_index = mark->prev;
                    break;
                }
                MD_FALLTHROUGH();
            default:
                mark_index--;
                break;
        }
    }
}