CotpConnection_sendConnectionResponseMessage(CotpConnection* self)
{
    allocateWriteBuffer(self);

    int optionsLength = getOptionsLength(self);
    int messageLength = 11 + optionsLength;

    writeRfc1006Header(self, messageLength);

    writeStaticConnectResponseHeader(self, optionsLength);

    writeOptions(self);

    if (sendBuffer(self))
        return COTP_OK;
    else
        return COTP_ERROR;
}