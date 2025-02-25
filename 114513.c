CotpConnection_readToTpktBuffer(CotpConnection* self)
{
    uint8_t* buffer = self->readBuffer->buffer;
    int bufferSize = self->readBuffer->maxSize;
    int bufPos = self->readBuffer->size;

    assert (bufferSize > 4);

    int readBytes;

    if (bufPos < 4) {

        readBytes = readFromSocket(self, buffer + bufPos, 4 - bufPos);

        if (readBytes < 0)
            goto exit_closed;

        if (DEBUG_COTP) {
            if (readBytes > 0)
                printf("TPKT: read %i bytes from socket\n", readBytes);
        }

        bufPos += readBytes;

        if (bufPos == 4) {
            if ((buffer[0] == 3) && (buffer[1] == 0)) {
                self->packetSize = (buffer[2] * 0x100) + buffer[3];

                if (DEBUG_COTP)
                    printf("TPKT: header complete (msg size = %i)\n", self->packetSize);

                if (self->packetSize > bufferSize) {
                    if (DEBUG_COTP) printf("TPKT: packet too large\n");
                    goto exit_error;
                }
            }
            else {
                if (DEBUG_COTP) printf("TPKT: failed to decode TPKT header.\n");
                goto exit_error;
            }
        }
        else
            goto exit_waiting;
    }

    if (self->packetSize <= bufPos)
        goto exit_error;

    readBytes = readFromSocket(self, buffer + bufPos, self->packetSize - bufPos);

    if (readBytes < 0)
        goto exit_closed;

    bufPos += readBytes;

    if (bufPos < self->packetSize)
       goto exit_waiting;

    if (DEBUG_COTP) printf("TPKT: message complete (size = %i)\n", self->packetSize);

    self->readBuffer->size = bufPos;
    return TPKT_PACKET_COMPLETE;

exit_closed:
    if (DEBUG_COTP) printf("TPKT: socket closed or socket error\n");
    return TPKT_ERROR;

exit_error:
    if (DEBUG_COTP) printf("TPKT: Error parsing message\n");
    return TPKT_ERROR;

exit_waiting:

    if (DEBUG_COTP)
        if (bufPos != 0)
            printf("TPKT: waiting (read %i of %i)\n", bufPos, self->packetSize);

    self->readBuffer->size = bufPos;
    return TPKT_WAITING;
}