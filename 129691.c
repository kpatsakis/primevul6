static void ehci_queues_rip_all(EHCIState *ehci, int async)
{
    EHCIQueueHead *head = async ? &ehci->aqueues : &ehci->pqueues;
    const char *warn = async ? "guest stopped busy async schedule" : NULL;
    EHCIQueue *q, *tmp;

    QTAILQ_FOREACH_SAFE(q, head, next, tmp) {
        ehci_free_queue(q, warn);
    }
}