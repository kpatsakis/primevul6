static void jpeg2000_flush(Jpeg2000DecoderContext *s)
{
    if (bytestream2_get_byte(&s->g) == 0xff)
        bytestream2_skip(&s->g, 1);
    s->bit_index = 8;
}