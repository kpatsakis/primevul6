mac_low16_read(E1000State *s, int index)
{
    return s->mac_reg[index] & 0xffff;
}