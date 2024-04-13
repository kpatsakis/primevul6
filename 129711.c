static void ehci_update_frindex(EHCIState *ehci, int uframes)
{
    if (!ehci_enabled(ehci) && ehci->pstate == EST_INACTIVE) {
        return;
    }

    /* Generate FLR interrupt if frame index rolls over 0x2000 */
    if ((ehci->frindex % 0x2000) + uframes >= 0x2000) {
        ehci_raise_irq(ehci, USBSTS_FLR);
    }

    /* How many times will frindex roll over 0x4000 with this frame count?
     * usbsts_frindex is decremented by 0x4000 on rollover until it reaches 0
     */
    int rollovers = (ehci->frindex + uframes) / 0x4000;
    if (rollovers > 0) {
        if (ehci->usbsts_frindex >= (rollovers * 0x4000)) {
            ehci->usbsts_frindex -= 0x4000 * rollovers;
        } else {
            ehci->usbsts_frindex = 0;
        }
    }

    ehci->frindex = (ehci->frindex + uframes) % 0x4000;
}