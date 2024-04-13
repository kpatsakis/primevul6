static void ehci_trace_qtd(EHCIQueue *q, hwaddr addr, EHCIqtd *qtd)
{
    /* need three here due to argument count limits */
    trace_usb_ehci_qtd_ptrs(q, addr, qtd->next, qtd->altnext);
    trace_usb_ehci_qtd_fields(addr,
                              get_field(qtd->token, QTD_TOKEN_TBYTES),
                              get_field(qtd->token, QTD_TOKEN_CPAGE),
                              get_field(qtd->token, QTD_TOKEN_CERR),
                              get_field(qtd->token, QTD_TOKEN_PID));
    trace_usb_ehci_qtd_bits(addr,
                            (bool)(qtd->token & QTD_TOKEN_IOC),
                            (bool)(qtd->token & QTD_TOKEN_ACTIVE),
                            (bool)(qtd->token & QTD_TOKEN_HALT),
                            (bool)(qtd->token & QTD_TOKEN_BABBLE),
                            (bool)(qtd->token & QTD_TOKEN_XACTERR));
}