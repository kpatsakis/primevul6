CotpConnection_init(CotpConnection* self, Socket socket,
        ByteBuffer* payloadBuffer, ByteBuffer* readBuffer, ByteBuffer* writeBuffer)
{
    self->state = 0;
    self->socket = socket;
#if (CONFIG_MMS_SUPPORT_TLS == 1)
    self->tlsSocket = NULL;
#endif
    self->remoteRef = -1;
    self->localRef = 1;
    self->protocolClass = -1;
	self->options.tpduSize = 0;

	TSelector tsel;
	tsel.size = 2;
	tsel.value[0] = 0;
	tsel.value[1] = 1;

	self->options.tSelSrc = tsel;
	self->options.tSelDst = tsel;
    self->payload = payloadBuffer;

    CotpConnection_resetPayload(self);

    /* default TPDU size is maximum size */
    CotpConnection_setTpduSize(self, COTP_MAX_TPDU_SIZE);

    self->writeBuffer = writeBuffer;
    self->readBuffer = readBuffer;
    self->packetSize = 0;
}