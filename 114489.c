CotpConnection_sendConnectionRequestMessage(CotpConnection* self, IsoConnectionParameters isoParameters)
{
    allocateWriteBuffer(self);

    self->options.tSelDst = isoParameters->remoteTSelector;
    self->options.tSelSrc = isoParameters->localTSelector;

    int cotpRequestSize = getOptionsLength(self) + 6;

    int conRequestSize = cotpRequestSize + 5;

    if(self->writeBuffer->maxSize < conRequestSize)
        return COTP_ERROR;

    uint8_t* buffer = self->writeBuffer->buffer;

    writeRfc1006Header(self, conRequestSize);

    /* LI */
    buffer[4] = (uint8_t) cotpRequestSize;

    /* TPDU CODE */
    buffer[5] = 0xe0;

    /* DST REF */
    buffer[6] = 0x00;
    buffer[7] = 0x00;

    /* SRC REF */
    buffer[8] = (uint8_t) (self->localRef / 0x100);
    buffer[9] = (uint8_t) (self->localRef & 0xff);

    /* Class */
    buffer[10] = 0x00;

    self->writeBuffer->size = 11;

    writeOptions(self);

    if (sendBuffer(self))
        return COTP_OK;
    else
        return COTP_ERROR;
}