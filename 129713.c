static void ehci_trace_usbsts(uint32_t mask, int state)
{
    /* interrupts */
    if (mask & USBSTS_INT) {
        trace_usb_ehci_usbsts("INT", state);
    }
    if (mask & USBSTS_ERRINT) {
        trace_usb_ehci_usbsts("ERRINT", state);
    }
    if (mask & USBSTS_PCD) {
        trace_usb_ehci_usbsts("PCD", state);
    }
    if (mask & USBSTS_FLR) {
        trace_usb_ehci_usbsts("FLR", state);
    }
    if (mask & USBSTS_HSE) {
        trace_usb_ehci_usbsts("HSE", state);
    }
    if (mask & USBSTS_IAA) {
        trace_usb_ehci_usbsts("IAA", state);
    }

    /* status */
    if (mask & USBSTS_HALT) {
        trace_usb_ehci_usbsts("HALT", state);
    }
    if (mask & USBSTS_REC) {
        trace_usb_ehci_usbsts("REC", state);
    }
    if (mask & USBSTS_PSS) {
        trace_usb_ehci_usbsts("PSS", state);
    }
    if (mask & USBSTS_ASS) {
        trace_usb_ehci_usbsts("ASS", state);
    }
}