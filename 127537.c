static uint8_t get_tlm(Jpeg2000DecoderContext *s, int n)
{
    uint8_t Stlm, ST, SP, tile_tlm, i;
    bytestream2_get_byte(&s->g);               /* Ztlm: skipped */
    Stlm = bytestream2_get_byte(&s->g);

    // too complex ? ST = ((Stlm >> 4) & 0x01) + ((Stlm >> 4) & 0x02);
    ST = (Stlm >> 4) & 0x03;
    // TODO: Manage case of ST = 0b11 --> raise error
    SP       = (Stlm >> 6) & 0x01;
    tile_tlm = (n - 4) / ((SP + 1) * 2 + ST);
    for (i = 0; i < tile_tlm; i++) {
        switch (ST) {
        case 0:
            break;
        case 1:
            bytestream2_get_byte(&s->g);
            break;
        case 2:
            bytestream2_get_be16(&s->g);
            break;
        case 3:
            bytestream2_get_be32(&s->g);
            break;
        }
        if (SP == 0) {
            bytestream2_get_be16(&s->g);
        } else {
            bytestream2_get_be32(&s->g);
        }
    }
    return 0;
}