static uint64_t ehci_caps_read(void *ptr, hwaddr addr,
                               unsigned size)
{
    EHCIState *s = ptr;
    return s->caps[addr];
}