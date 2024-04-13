writeOptions(CotpConnection* self)
{
    /* max size = 11 byte */
    uint8_t* buffer = self->writeBuffer->buffer;
    int bufPos = self->writeBuffer->size;

    if (self->options.tpduSize != 0) {

        if (DEBUG_COTP)
            printf("COTP: send TPDU size: %i\n", CotpConnection_getTpduSize(self));

        buffer[bufPos++] = 0xc0;
        buffer[bufPos++] = 0x01;
        buffer[bufPos++] = self->options.tpduSize;
    }

    if (self->options.tSelDst.size != 0) {
        buffer[bufPos++] = 0xc2;
        buffer[bufPos++] = (uint8_t) self->options.tSelDst.size;

        int i;
        for (i = 0; i < self->options.tSelDst.size; i++)
            buffer[bufPos++] = (uint8_t) self->options.tSelDst.value[i];
    }

    if (self->options.tSelSrc.size != 0) {
        buffer[bufPos++] = 0xc1;
        buffer[bufPos++] = (uint8_t) self->options.tSelSrc.size;

        int i;
        for (i = 0; i < self->options.tSelSrc.size; i++)
            buffer[bufPos++] = (uint8_t) self->options.tSelSrc.value[i];
    }

    self->writeBuffer->size = bufPos;
}