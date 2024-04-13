static bool ehci_verify_qh(EHCIQueue *q, EHCIqh *qh)
{
    uint32_t devaddr = get_field(qh->epchar, QH_EPCHAR_DEVADDR);
    uint32_t endp    = get_field(qh->epchar, QH_EPCHAR_EP);
    if ((devaddr != get_field(q->qh.epchar, QH_EPCHAR_DEVADDR)) ||
        (endp    != get_field(q->qh.epchar, QH_EPCHAR_EP)) ||
        (qh->current_qtd != q->qh.current_qtd) ||
        (q->async && qh->next_qtd != q->qh.next_qtd) ||
        (memcmp(&qh->altnext_qtd, &q->qh.altnext_qtd,
                                 7 * sizeof(uint32_t)) != 0) ||
        (q->dev != NULL && q->dev->addr != devaddr)) {
        return false;
    } else {
        return true;
    }
}