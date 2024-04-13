allocateWriteBuffer(CotpConnection* self)
{
    if (self->writeBuffer == NULL )
        self->writeBuffer = ByteBuffer_create(NULL,
                CotpConnection_getTpduSize(self) + TPKT_RFC1006_HEADER_SIZE);
}