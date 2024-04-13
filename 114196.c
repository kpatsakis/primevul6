mac_low11_read(E1000State *s, int index)
{
    return s->mac_reg[index] & 0x7ff;
}