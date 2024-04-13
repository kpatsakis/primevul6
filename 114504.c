parseConnectConfirmTpdu(CotpConnection* self, uint8_t* buffer, uint8_t len)
{
    if (len < 6)
        return false;

    self->remoteRef = getUint16(buffer);
    self->protocolClass = getUint8(buffer + 4);

    return parseOptions(self, buffer + 5, len - 6);
}