static void ehci_trace_guest_bug(EHCIState *s, const char *message)
{
    trace_usb_ehci_guest_bug(message);
    fprintf(stderr, "ehci warning: %s\n", message);
}