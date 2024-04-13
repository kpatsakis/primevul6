RecordSanityCheckRegisterClients(RecordContextPtr pContext, ClientPtr client,
                                 xRecordRegisterClientsReq * stuff)
{
    int err;
    xRecordRange *pRange;
    int i;
    XID recordingClient;

    if (((client->req_len << 2) - SIZEOF(xRecordRegisterClientsReq)) !=
        4 * stuff->nClients + SIZEOF(xRecordRange) * stuff->nRanges)
        return BadLength;

    if (stuff->elementHeader &
        ~(XRecordFromClientSequence | XRecordFromClientTime |
          XRecordFromServerTime)) {
        client->errorValue = stuff->elementHeader;
        return BadValue;
    }

    recordingClient = pContext->pRecordingClient ?
        pContext->pRecordingClient->clientAsMask : 0;
    err = RecordSanityCheckClientSpecifiers(client, (XID *) &stuff[1],
                                            stuff->nClients, recordingClient);
    if (err != Success)
        return err;

    pRange = (xRecordRange *) (((XID *) &stuff[1]) + stuff->nClients);
    for (i = 0; i < stuff->nRanges; i++, pRange++) {
        if (pRange->coreRequestsFirst > pRange->coreRequestsLast) {
            client->errorValue = pRange->coreRequestsFirst;
            return BadValue;
        }
        if (pRange->coreRepliesFirst > pRange->coreRepliesLast) {
            client->errorValue = pRange->coreRepliesFirst;
            return BadValue;
        }
        if ((pRange->extRequestsMajorFirst || pRange->extRequestsMajorLast) &&
            (pRange->extRequestsMajorFirst < 128 ||
             pRange->extRequestsMajorLast < 128 ||
             pRange->extRequestsMajorFirst > pRange->extRequestsMajorLast)) {
            client->errorValue = pRange->extRequestsMajorFirst;
            return BadValue;
        }
        if (pRange->extRequestsMinorFirst > pRange->extRequestsMinorLast) {
            client->errorValue = pRange->extRequestsMinorFirst;
            return BadValue;
        }
        if ((pRange->extRepliesMajorFirst || pRange->extRepliesMajorLast) &&
            (pRange->extRepliesMajorFirst < 128 ||
             pRange->extRepliesMajorLast < 128 ||
             pRange->extRepliesMajorFirst > pRange->extRepliesMajorLast)) {
            client->errorValue = pRange->extRepliesMajorFirst;
            return BadValue;
        }
        if (pRange->extRepliesMinorFirst > pRange->extRepliesMinorLast) {
            client->errorValue = pRange->extRepliesMinorFirst;
            return BadValue;
        }
        if ((pRange->deliveredEventsFirst || pRange->deliveredEventsLast) &&
            (pRange->deliveredEventsFirst < 2 ||
             pRange->deliveredEventsLast < 2 ||
             pRange->deliveredEventsFirst > pRange->deliveredEventsLast)) {
            client->errorValue = pRange->deliveredEventsFirst;
            return BadValue;
        }
        if ((pRange->deviceEventsFirst || pRange->deviceEventsLast) &&
            (pRange->deviceEventsFirst < 2 ||
             pRange->deviceEventsLast < 2 ||
             pRange->deviceEventsFirst > pRange->deviceEventsLast)) {
            client->errorValue = pRange->deviceEventsFirst;
            return BadValue;
        }
        if (pRange->errorsFirst > pRange->errorsLast) {
            client->errorValue = pRange->errorsFirst;
            return BadValue;
        }
        if (pRange->clientStarted != xFalse && pRange->clientStarted != xTrue) {
            client->errorValue = pRange->clientStarted;
            return BadValue;
        }
        if (pRange->clientDied != xFalse && pRange->clientDied != xTrue) {
            client->errorValue = pRange->clientDied;
            return BadValue;
        }
    }                           /* end for each range */
    return Success;
}                               /* end RecordSanityCheckRegisterClients */