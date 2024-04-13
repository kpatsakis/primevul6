static void ehci_writeback_async_complete_packet(EHCIPacket *p)
{
    EHCIQueue *q = p->queue;
    EHCIqtd qtd;
    EHCIqh qh;
    int state;

    /* Verify the qh + qtd, like we do when going through fetchqh & fetchqtd */
    get_dwords(q->ehci, NLPTR_GET(q->qhaddr),
               (uint32_t *) &qh, sizeof(EHCIqh) >> 2);
    get_dwords(q->ehci, NLPTR_GET(q->qtdaddr),
               (uint32_t *) &qtd, sizeof(EHCIqtd) >> 2);
    if (!ehci_verify_qh(q, &qh) || !ehci_verify_qtd(p, &qtd)) {
        p->async = EHCI_ASYNC_INITIALIZED;
        ehci_free_packet(p);
        return;
    }

    state = ehci_get_state(q->ehci, q->async);
    ehci_state_executing(q);
    ehci_state_writeback(q); /* Frees the packet! */
    if (!(q->qh.token & QTD_TOKEN_HALT)) {
        ehci_state_advqueue(q);
    }
    ehci_set_state(q->ehci, q->async, state);
}