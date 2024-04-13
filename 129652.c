static void ehci_queues_rip_device(EHCIState *ehci, USBDevice *dev, int async)
{
    EHCIQueueHead *head = async ? &ehci->aqueues : &ehci->pqueues;
    EHCIQueue *q, *tmp;

    QTAILQ_FOREACH_SAFE(q, head, next, tmp) {
        if (q->dev != dev) {
            continue;
        }
        ehci_free_queue(q, NULL);
    }
}