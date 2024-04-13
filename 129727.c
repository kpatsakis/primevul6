static int ehci_get_state(EHCIState *s, int async)
{
    return async ? s->astate : s->pstate;
}