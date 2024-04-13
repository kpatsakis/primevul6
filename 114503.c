sendBuffer(CotpConnection* self)
{
    int remainingSize = ByteBuffer_getSize(self->writeBuffer);
    uint8_t* buffer = ByteBuffer_getBuffer(self->writeBuffer);

    bool retVal = false;

    do {
        int sentBytes = writeToSocket(self, buffer, remainingSize);

        if (sentBytes == -1)
            goto exit_function;

        buffer += sentBytes;
        remainingSize -= sentBytes;

    } while (remainingSize > 0);

    retVal = true;

    ByteBuffer_setSize(self->writeBuffer, 0);

exit_function:
    return retVal;
}