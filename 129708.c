static void ehci_finish_transfer(EHCIQueue *q, int len)
{
    uint32_t cpage, offset;

    if (len > 0) {
        /* update cpage & offset */
        cpage  = get_field(q->qh.token, QTD_TOKEN_CPAGE);
        offset = q->qh.bufptr[0] & ~QTD_BUFPTR_MASK;

        offset += len;
        cpage  += offset >> QTD_BUFPTR_SH;
        offset &= ~QTD_BUFPTR_MASK;

        set_field(&q->qh.token, cpage, QTD_TOKEN_CPAGE);
        q->qh.bufptr[0] &= QTD_BUFPTR_MASK;
        q->qh.bufptr[0] |= offset;
    }
}