static void ehci_queues_rip_unseen(EHCIState *ehci, int async)
{
    EHCIQueueHead *head = async ? &ehci->aqueues : &ehci->pqueues;
    EHCIQueue *q, *tmp;

    QTAILQ_FOREACH_SAFE(q, head, next, tmp) {
        if (!q->seen) {
            ehci_free_queue(q, NULL);
        }
    }
}