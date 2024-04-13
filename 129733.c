static const char *addr2str(hwaddr addr)
{
    return nr2str(ehci_mmio_names, ARRAY_SIZE(ehci_mmio_names), addr);
}