CotpConnection_sendDataMessage(CotpConnection* self, BufferChain payload)
{
    CotpIndication retValue = COTP_OK;

    int fragments = 1;

    int fragmentPayloadSize = CotpConnection_getTpduSize(self) - COTP_DATA_HEADER_SIZE;

    if (payload->length > fragmentPayloadSize) { /* Check if segmentation is required? */
        fragments = payload->length / fragmentPayloadSize;

        if ((payload->length % fragmentPayloadSize) != 0)
            fragments += 1;
    }

    int currentBufPos = 0;
    int currentLimit;
    int lastUnit;

    BufferChain currentChain = payload;
    int currentChainIndex = 0;

    if (DEBUG_COTP)
        printf("\nCOTP: nextBufferPart: len:%i partLen:%i\n", currentChain->length, currentChain->partLength);

    uint8_t* buffer = self->writeBuffer->buffer;

    while (fragments > 0) {
        if (fragments > 1) {
            currentLimit = currentBufPos + fragmentPayloadSize;
            lastUnit = 0;
        }
        else {
            currentLimit = payload->length;
            lastUnit = 1;
        }

        writeRfc1006Header(self, 7 + (currentLimit - currentBufPos));

        writeDataTpduHeader(self, lastUnit);

        int bufPos = 7;

        int i;
        for (i = currentBufPos; i < currentLimit; i++) {

            if (currentChainIndex >= currentChain->partLength) {
                currentChain = currentChain->nextPart;
                if (DEBUG_COTP)
                    printf("\nCOTP: nextBufferPart: len:%i partLen:%i\n", currentChain->length, currentChain->partLength);
                currentChainIndex = 0;
            }

            buffer[bufPos++] = currentChain->buffer[currentChainIndex];

            currentChainIndex++;

            currentBufPos++;
        }

        self->writeBuffer->size = bufPos;

        if (DEBUG_COTP)
            printf("COTP: Send COTP fragment %i bufpos: %i\n", fragments, currentBufPos);

        if (!sendBuffer(self)) {
            retValue = COTP_ERROR;

            if (DEBUG_COTP)
                printf("COTP: sending message failed!\n");

            goto exit_function;
        }


        fragments--;
    }

exit_function:

    if (DEBUG_COTP)
        printf("COTP: message transmission finished (fragments=%i, return=%i)\n", fragments, retValue);

    return retValue;
}