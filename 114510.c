CotpConnection_parseIncomingMessage(CotpConnection* self)
{
    CotpIndication indication = parseCotpMessage(self);

    self->readBuffer->size = 0;
    self->packetSize = 0;

    return indication;
}