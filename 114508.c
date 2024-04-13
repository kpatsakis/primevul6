writeRfc1006Header(CotpConnection* self, int len)
{
    uint8_t* buffer = self->writeBuffer->buffer;

    buffer[0] = 0x03;
    buffer[1] = 0x00;
    buffer[2] = (uint8_t) (len / 0x100);
    buffer[3] = (uint8_t) (len & 0xff);

    self->writeBuffer->size = 4;
}