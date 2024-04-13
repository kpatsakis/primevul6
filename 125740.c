njs_chb_write_byte_as_utf8(njs_chb_t *chain, u_char byte)
{
    njs_utf8_encode(njs_chb_current(chain), byte);
    njs_chb_written(chain, njs_utf8_size(byte));
}