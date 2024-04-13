gif_get16le(gif_context_t *s)
{
    int z = gif_get8(s);
    return z + (gif_get8(s) << 8);
}