static inline bool ehci_async_enabled(EHCIState *s)
{
    return ehci_enabled(s) && (s->usbcmd & USBCMD_ASE);
}