static void usb_ehci_pre_save(void *opaque)
{
    EHCIState *ehci = opaque;
    uint32_t new_frindex;

    /* Round down frindex to a multiple of 8 for migration compatibility */
    new_frindex = ehci->frindex & ~7;
    ehci->last_run_ns -= (ehci->frindex - new_frindex) * UFRAME_TIMER_NS;
    ehci->frindex = new_frindex;
}