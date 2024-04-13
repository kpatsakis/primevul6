addPayloadToBuffer(CotpConnection* self, uint8_t* buffer,  int payloadLength)
{
    if (payloadLength < 1) {
        if (DEBUG_COTP)
            printf("COTP: missing payload\n");

        return false;
    }

    if (DEBUG_COTP)
        printf("COTP: add to payload buffer (cur size: %i, len: %i)\n", self->payload->size, payloadLength);

    if ((self->payload->size + payloadLength) > self->payload->maxSize)
        return false;

    memcpy(self->payload->buffer + self->payload->size, buffer, payloadLength);

    self->payload->size += payloadLength;

    return true;
}