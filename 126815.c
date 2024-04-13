static av_noinline int get_symbol(RangeCoder *c, uint8_t *state, int is_signed)
{
    return get_symbol_inline(c, state, is_signed);
}