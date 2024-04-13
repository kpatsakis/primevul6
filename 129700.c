static void ehci_queue_stopped(EHCIQueue *q)
{
    int endp  = get_field(q->qh.epchar, QH_EPCHAR_EP);

    if (!q->last_pid || !q->dev) {
        return;
    }

    usb_device_ep_stopped(q->dev, usb_ep_get(q->dev, q->last_pid, endp));
}