RecordAProtocolElement(RecordContextPtr pContext, ClientPtr pClient,
                       int category, void *data, int datalen, int padlen,
                       int futurelen)
{
    CARD32 elemHeaderData[2];
    int numElemHeaders = 0;
    Bool recordingClientSwapped = pContext->pRecordingClient->swapped;
    CARD32 serverTime = 0;
    Bool gotServerTime = FALSE;
    int replylen;

    if (futurelen >= 0) {       /* start of new protocol element */
        xRecordEnableContextReply *pRep = (xRecordEnableContextReply *)
            pContext->replyBuffer;

        if (pContext->pBufClient != pClient ||
            pContext->bufCategory != category) {
            RecordFlushReplyBuffer(pContext, NULL, 0, NULL, 0);
            pContext->pBufClient = pClient;
            pContext->bufCategory = category;
        }

        if (!pContext->numBufBytes) {
            serverTime = GetTimeInMillis();
            gotServerTime = TRUE;
            pRep->type = X_Reply;
            pRep->category = category;
            pRep->sequenceNumber = pContext->pRecordingClient->sequence;
            pRep->length = 0;
            pRep->elementHeader = pContext->elemHeaders;
            pRep->serverTime = serverTime;
            if (pClient) {
                pRep->clientSwapped =
                    (pClient->swapped != recordingClientSwapped);
                pRep->idBase = pClient->clientAsMask;
                pRep->recordedSequenceNumber = pClient->sequence;
            }
            else {              /* it's a device event, StartOfData, or EndOfData */

                pRep->clientSwapped = (category != XRecordFromServer) &&
                    recordingClientSwapped;
                pRep->idBase = 0;
                pRep->recordedSequenceNumber = 0;
            }

            if (recordingClientSwapped) {
                swaps(&pRep->sequenceNumber);
                swapl(&pRep->length);
                swapl(&pRep->idBase);
                swapl(&pRep->serverTime);
                swapl(&pRep->recordedSequenceNumber);
            }
            pContext->numBufBytes = SIZEOF(xRecordEnableContextReply);
        }

        /* generate element headers if needed */

        if (((pContext->elemHeaders & XRecordFromClientTime)
             && category == XRecordFromClient)
            || ((pContext->elemHeaders & XRecordFromServerTime)
                && category == XRecordFromServer)) {
            if (gotServerTime)
                elemHeaderData[numElemHeaders] = serverTime;
            else
                elemHeaderData[numElemHeaders] = GetTimeInMillis();
            if (recordingClientSwapped)
                swapl(&elemHeaderData[numElemHeaders]);
            numElemHeaders++;
        }

        if ((pContext->elemHeaders & XRecordFromClientSequence)
            && (category == XRecordFromClient || category == XRecordClientDied)) {
            elemHeaderData[numElemHeaders] = pClient->sequence;
            if (recordingClientSwapped)
                swapl(&elemHeaderData[numElemHeaders]);
            numElemHeaders++;
        }

        /* adjust reply length */

        replylen = pRep->length;
        if (recordingClientSwapped)
            swapl(&replylen);
        replylen += numElemHeaders + bytes_to_int32(datalen) +
            bytes_to_int32(futurelen);
        if (recordingClientSwapped)
            swapl(&replylen);
        pRep->length = replylen;
    }                           /* end if not continued reply */

    numElemHeaders *= 4;

    /* if space available >= space needed, buffer the data */

    if (REPLY_BUF_SIZE - pContext->numBufBytes >= datalen + numElemHeaders) {
        if (numElemHeaders) {
            memcpy(pContext->replyBuffer + pContext->numBufBytes,
                   elemHeaderData, numElemHeaders);
            pContext->numBufBytes += numElemHeaders;
        }
        if (datalen) {
            static char padBuffer[3];   /* as in FlushClient */

            memcpy(pContext->replyBuffer + pContext->numBufBytes,
                   data, datalen - padlen);
            pContext->numBufBytes += datalen - padlen;
            memcpy(pContext->replyBuffer + pContext->numBufBytes,
                   padBuffer, padlen);
            pContext->numBufBytes += padlen;
        }
    }
    else {
        RecordFlushReplyBuffer(pContext, (void *) elemHeaderData,
                               numElemHeaders, (void *) data,
                               datalen - padlen);
    }
}                               /* RecordAProtocolElement */