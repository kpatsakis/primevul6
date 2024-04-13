md_mark_chain_append(MD_CTX* ctx, MD_MARKCHAIN* chain, int mark_index)
{
    if(chain->tail >= 0)
        ctx->marks[chain->tail].next = mark_index;
    else
        chain->head = mark_index;

    ctx->marks[mark_index].prev = chain->tail;
    ctx->marks[mark_index].next = -1;
    chain->tail = mark_index;
}