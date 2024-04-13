static void ehci_update_halt(EHCIState *s)
{
    if (s->usbcmd & USBCMD_RUNSTOP) {
        ehci_clear_usbsts(s, USBSTS_HALT);
    } else {
        if (s->astate == EST_INACTIVE && s->pstate == EST_INACTIVE) {
            ehci_set_usbsts(s, USBSTS_HALT);
        }
    }
}