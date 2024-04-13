md_resolve_range(MD_CTX* ctx, MD_MARKCHAIN* chain, int opener_index, int closer_index)
{
    MD_MARK* opener = &ctx->marks[opener_index];
    MD_MARK* closer = &ctx->marks[closer_index];

    /* Remove opener from the list of openers. */
    if(chain != NULL) {
        if(opener->prev >= 0)
            ctx->marks[opener->prev].next = opener->next;
        else
            chain->head = opener->next;

        if(opener->next >= 0)
            ctx->marks[opener->next].prev = opener->prev;
        else
            chain->tail = opener->prev;
    }

    /* Interconnect opener and closer and mark both as resolved. */
    opener->next = closer_index;
    opener->flags |= MD_MARK_OPENER | MD_MARK_RESOLVED;
    closer->prev = opener_index;
    closer->flags |= MD_MARK_CLOSER | MD_MARK_RESOLVED;
}