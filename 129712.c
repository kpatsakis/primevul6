static void ehci_trace_qh(EHCIQueue *q, hwaddr addr, EHCIqh *qh)
{
    /* need three here due to argument count limits */
    trace_usb_ehci_qh_ptrs(q, addr, qh->next,
                           qh->current_qtd, qh->next_qtd, qh->altnext_qtd);
    trace_usb_ehci_qh_fields(addr,
                             get_field(qh->epchar, QH_EPCHAR_RL),
                             get_field(qh->epchar, QH_EPCHAR_MPLEN),
                             get_field(qh->epchar, QH_EPCHAR_EPS),
                             get_field(qh->epchar, QH_EPCHAR_EP),
                             get_field(qh->epchar, QH_EPCHAR_DEVADDR));
    trace_usb_ehci_qh_bits(addr,
                           (bool)(qh->epchar & QH_EPCHAR_C),
                           (bool)(qh->epchar & QH_EPCHAR_H),
                           (bool)(qh->epchar & QH_EPCHAR_DTC),
                           (bool)(qh->epchar & QH_EPCHAR_I));
}