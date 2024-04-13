static int ehci_get_fetch_addr(EHCIState *s, int async)
{
    return async ? s->a_fetch_addr : s->p_fetch_addr;
}