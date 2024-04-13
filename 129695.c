static inline bool ehci_periodic_enabled(EHCIState *s)
{
    return ehci_enabled(s) && (s->usbcmd & USBCMD_PSE);
}