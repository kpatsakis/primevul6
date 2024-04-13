static void ehci_set_state(EHCIState *s, int async, int state)
{
    if (async) {
        trace_usb_ehci_state("async", state2str(state));
        s->astate = state;
        if (s->astate == EST_INACTIVE) {
            ehci_clear_usbsts(s, USBSTS_ASS);
            ehci_update_halt(s);
        } else {
            ehci_set_usbsts(s, USBSTS_ASS);
        }
    } else {
        trace_usb_ehci_state("periodic", state2str(state));
        s->pstate = state;
        if (s->pstate == EST_INACTIVE) {
            ehci_clear_usbsts(s, USBSTS_PSS);
            ehci_update_halt(s);
        } else {
            ehci_set_usbsts(s, USBSTS_PSS);
        }
    }
}