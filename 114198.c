mac_low13_read(E1000State *s, int index)
{
    return s->mac_reg[index] & 0x1fff;
}