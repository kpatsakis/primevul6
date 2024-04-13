parseCotpMessage(CotpConnection* self)
{
    uint8_t* buffer = self->readBuffer->buffer + 4;
    int tpduLength = self->readBuffer->size - 4;

    uint8_t len;
    uint8_t tpduType;

    len = buffer[0];

    if (len > tpduLength) {
      if (DEBUG_COTP)
          printf("COTP: parseCotpMessage: len=%d tpduLength=%d\n", len, tpduLength);

      return COTP_ERROR;
    }

    tpduType = buffer[1];

    switch (tpduType) {
    case 0xe0:
        if (parseConnectRequestTpdu(self, buffer + 2, len))
            return COTP_CONNECT_INDICATION;
        else
            return COTP_ERROR;
    case 0xd0:
        if (parseConnectConfirmTpdu(self, buffer + 2, len))
            return COTP_CONNECT_INDICATION;
        else
            return COTP_ERROR;
    case 0xf0:
        if (parseDataTpdu(self, buffer + 2, len)) {

            if (addPayloadToBuffer(self, buffer + 3, tpduLength - 3) != 1)
                return COTP_ERROR;

            if (self->isLastDataUnit)
                return COTP_DATA_INDICATION;
            else
                return COTP_MORE_FRAGMENTS_FOLLOW;
        }
        else
            return COTP_ERROR;
    default:
        return COTP_ERROR;
    }

}