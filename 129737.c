static bool ehci_verify_qtd(EHCIPacket *p, EHCIqtd *qtd)
{
    if (p->qtdaddr != p->queue->qtdaddr ||
        (p->queue->async && !NLPTR_TBIT(p->qtd.next) &&
            (p->qtd.next != qtd->next)) ||
        (!NLPTR_TBIT(p->qtd.altnext) && (p->qtd.altnext != qtd->altnext)) ||
        p->qtd.token != qtd->token ||
        p->qtd.bufptr[0] != qtd->bufptr[0]) {
        return false;
    } else {
        return true;
    }
}