static const char *state2str(uint32_t state)
{
    return nr2str(ehci_state_names, ARRAY_SIZE(ehci_state_names), state);
}