mac_low4_read(E1000State *s, int index)
{
    return s->mac_reg[index] & 0xf;
}