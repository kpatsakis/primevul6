static void ehci_trace_sitd(EHCIState *s, hwaddr addr,
                            EHCIsitd *sitd)
{
    trace_usb_ehci_sitd(addr, sitd->next,
                        (bool)(sitd->results & SITD_RESULTS_ACTIVE));
}