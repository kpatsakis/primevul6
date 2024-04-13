parseDataTpdu(CotpConnection* self, uint8_t* buffer, uint8_t len)
{
    if (len != 2)
        return false;

    uint8_t flowControl = getUint8(buffer);

    if (flowControl & 0x80)
        self->isLastDataUnit = true;
    else
        self->isLastDataUnit = false;

    return true;
}