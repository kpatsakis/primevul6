static void usb_ehci_vm_state_change(void *opaque, int running, RunState state)
{
    EHCIState *ehci = opaque;

    /*
     * We don't migrate the EHCIQueue-s, instead we rebuild them for the
     * schedule in guest memory. We must do the rebuilt ASAP, so that
     * USB-devices which have async handled packages have a packet in the
     * ep queue to match the completion with.
     */
    if (state == RUN_STATE_RUNNING) {
        ehci_advance_async_state(ehci);
    }

    /*
     * The schedule rebuilt from guest memory could cause the migration dest
     * to miss a QH unlink, and fail to cancel packets, since the unlinked QH
     * will never have existed on the destination. Therefor we must flush the
     * async schedule on savevm to catch any not yet noticed unlinks.
     */
    if (state == RUN_STATE_SAVE_VM) {
        ehci_advance_async_state(ehci);
        ehci_queues_rip_unseen(ehci, 1);
    }
}