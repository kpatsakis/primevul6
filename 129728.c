static int ehci_state_advqueue(EHCIQueue *q)
{
#if 0
    /* TO-DO: 4.10.2 - paragraph 2
     * if I-bit is set to 1 and QH is not active
     * go to horizontal QH
     */
    if (I-bit set) {
        ehci_set_state(ehci, async, EST_HORIZONTALQH);
        goto out;
    }
#endif

    /*
     * want data and alt-next qTD is valid
     */
    if (((q->qh.token & QTD_TOKEN_TBYTES_MASK) != 0) &&
        (NLPTR_TBIT(q->qh.altnext_qtd) == 0)) {
        q->qtdaddr = q->qh.altnext_qtd;
        ehci_set_state(q->ehci, q->async, EST_FETCHQTD);

    /*
     *  next qTD is valid
     */
    } else if (NLPTR_TBIT(q->qh.next_qtd) == 0) {
        q->qtdaddr = q->qh.next_qtd;
        ehci_set_state(q->ehci, q->async, EST_FETCHQTD);

    /*
     *  no valid qTD, try next QH
     */
    } else {
        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);
    }

    return 1;
}