static int ehci_qh_do_overlay(EHCIQueue *q)
{
    EHCIPacket *p = QTAILQ_FIRST(&q->packets);
    int i;
    int dtoggle;
    int ping;
    int eps;
    int reload;

    assert(p != NULL);
    assert(p->qtdaddr == q->qtdaddr);

    // remember values in fields to preserve in qh after overlay

    dtoggle = q->qh.token & QTD_TOKEN_DTOGGLE;
    ping    = q->qh.token & QTD_TOKEN_PING;

    q->qh.current_qtd = p->qtdaddr;
    q->qh.next_qtd    = p->qtd.next;
    q->qh.altnext_qtd = p->qtd.altnext;
    q->qh.token       = p->qtd.token;


    eps = get_field(q->qh.epchar, QH_EPCHAR_EPS);
    if (eps == EHCI_QH_EPS_HIGH) {
        q->qh.token &= ~QTD_TOKEN_PING;
        q->qh.token |= ping;
    }

    reload = get_field(q->qh.epchar, QH_EPCHAR_RL);
    set_field(&q->qh.altnext_qtd, reload, QH_ALTNEXT_NAKCNT);

    for (i = 0; i < 5; i++) {
        q->qh.bufptr[i] = p->qtd.bufptr[i];
    }

    if (!(q->qh.epchar & QH_EPCHAR_DTC)) {
        // preserve QH DT bit
        q->qh.token &= ~QTD_TOKEN_DTOGGLE;
        q->qh.token |= dtoggle;
    }

    q->qh.bufptr[1] &= ~BUFPTR_CPROGMASK_MASK;
    q->qh.bufptr[2] &= ~BUFPTR_FRAMETAG_MASK;

    ehci_flush_qh(q);

    return 0;
}