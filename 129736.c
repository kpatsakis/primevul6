static EHCIQueue *ehci_find_queue_by_qh(EHCIState *ehci, uint32_t addr,
                                        int async)
{
    EHCIQueueHead *head = async ? &ehci->aqueues : &ehci->pqueues;
    EHCIQueue *q;

    QTAILQ_FOREACH(q, head, next) {
        if (addr == q->qhaddr) {
            return q;
        }
    }
    return NULL;
}