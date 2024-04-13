static int ehci_get_pid(EHCIqtd *qtd)
{
    switch (get_field(qtd->token, QTD_TOKEN_PID)) {
    case 0:
        return USB_TOKEN_OUT;
    case 1:
        return USB_TOKEN_IN;
    case 2:
        return USB_TOKEN_SETUP;
    default:
        fprintf(stderr, "bad token\n");
        return 0;
    }
}