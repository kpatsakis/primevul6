static void ehci_set_fetch_addr(EHCIState *s, int async, uint32_t addr)
{
    if (async) {
        s->a_fetch_addr = addr;
    } else {
        s->p_fetch_addr = addr;
    }
}