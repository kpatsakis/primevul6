static void ehci_trace_itd(EHCIState *s, hwaddr addr, EHCIitd *itd)
{
    trace_usb_ehci_itd(addr, itd->next,
                       get_field(itd->bufptr[1], ITD_BUFPTR_MAXPKT),
                       get_field(itd->bufptr[2], ITD_BUFPTR_MULT),
                       get_field(itd->bufptr[0], ITD_BUFPTR_EP),
                       get_field(itd->bufptr[0], ITD_BUFPTR_DEVADDR));
}