static void ehci_free_queue(EHCIQueue *q, const char *warn)
{
    EHCIQueueHead *head = q->async ? &q->ehci->aqueues : &q->ehci->pqueues;
    int cancelled;

    trace_usb_ehci_queue_action(q, "free");
    cancelled = ehci_cancel_queue(q);
    if (warn && cancelled > 0) {
        ehci_trace_guest_bug(q->ehci, warn);
    }
    QTAILQ_REMOVE(head, q, next);
    g_free(q);
}