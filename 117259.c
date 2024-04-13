ProcRecordGetContext(ClientPtr client)
{
    RecordContextPtr pContext;

    REQUEST(xRecordGetContextReq);
    xRecordGetContextReply rep;
    RecordClientsAndProtocolPtr pRCAP;
    int nRCAPs = 0;
    GetContextRangeInfoPtr pRangeInfo;
    GetContextRangeInfoPtr pri;
    int i;
    int err;
    CARD32 nClients, length;

    REQUEST_SIZE_MATCH(xRecordGetContextReq);
    VERIFY_CONTEXT(pContext, stuff->context, client);

    /* how many RCAPs are there on this context? */

    for (pRCAP = pContext->pListOfRCAP; pRCAP; pRCAP = pRCAP->pNextRCAP)
        nRCAPs++;

    /* allocate and initialize space for record range info */

    pRangeInfo = xallocarray(nRCAPs, sizeof(GetContextRangeInfoRec));
    if (!pRangeInfo && nRCAPs > 0)
        return BadAlloc;
    for (i = 0; i < nRCAPs; i++) {
        pRangeInfo[i].pRanges = NULL;
        pRangeInfo[i].size = 0;
        pRangeInfo[i].nRanges = 0;
    }

    /* convert the RCAP (internal) representation of the recorded protocol
     * to the wire protocol (external) representation, storing the information
     * for the ith RCAP in pri[i]
     */

    for (pRCAP = pContext->pListOfRCAP, pri = pRangeInfo;
         pRCAP; pRCAP = pRCAP->pNextRCAP, pri++) {
        xRecordRange rr;

        err = RecordConvertSetToRanges(pRCAP->pRequestMajorOpSet, pri,
                                       offset_of(rr, coreRequestsFirst), TRUE,
                                       127, NULL);
        if (err != Success)
            goto bailout;

        err = RecordConvertSetToRanges(pRCAP->pReplyMajorOpSet, pri,
                                       offset_of(rr, coreRepliesFirst), TRUE,
                                       127, NULL);
        if (err != Success)
            goto bailout;

        err = RecordConvertSetToRanges(pRCAP->pDeliveredEventSet, pri,
                                       offset_of(rr, deliveredEventsFirst),
                                       TRUE, 255, NULL);
        if (err != Success)
            goto bailout;

        err = RecordConvertSetToRanges(pRCAP->pDeviceEventSet, pri,
                                       offset_of(rr, deviceEventsFirst), TRUE,
                                       255, NULL);
        if (err != Success)
            goto bailout;

        err = RecordConvertSetToRanges(pRCAP->pErrorSet, pri,
                                       offset_of(rr, errorsFirst), TRUE, 255,
                                       NULL);
        if (err != Success)
            goto bailout;

        err = RecordConvertMinorOpInfoToRanges(pRCAP->pRequestMinOpInfo,
                                               pri, offset_of(rr,
                                                              extRequestsMajorFirst));
        if (err != Success)
            goto bailout;

        err = RecordConvertMinorOpInfoToRanges(pRCAP->pReplyMinOpInfo,
                                               pri, offset_of(rr,
                                                              extRepliesMajorFirst));
        if (err != Success)
            goto bailout;

        if (pRCAP->clientStarted || pRCAP->clientDied) {
            if (pri->nRanges == 0)
                RecordAllocRanges(pri, 1);
            pri->pRanges[0].clientStarted = pRCAP->clientStarted;
            pri->pRanges[0].clientDied = pRCAP->clientDied;
        }
    }

    /* calculate number of clients and reply length */

    nClients = 0;
    length = 0;
    for (pRCAP = pContext->pListOfRCAP, pri = pRangeInfo;
         pRCAP; pRCAP = pRCAP->pNextRCAP, pri++) {
        nClients += pRCAP->numClients;
        length += pRCAP->numClients *
            (bytes_to_int32(sizeof(xRecordClientInfo)) +
             pri->nRanges * bytes_to_int32(sizeof(xRecordRange)));
    }

    /* write the reply header */

    rep = (xRecordGetContextReply) {
        .type = X_Reply,
        .enabled = pContext->pRecordingClient != NULL,
        .sequenceNumber = client->sequence,
        .length = length,
        .elementHeader = pContext->elemHeaders,
        .nClients = nClients
    };
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.nClients);
    }
    WriteToClient(client, sizeof(xRecordGetContextReply), &rep);

    /* write all the CLIENT_INFOs */

    for (pRCAP = pContext->pListOfRCAP, pri = pRangeInfo;
         pRCAP; pRCAP = pRCAP->pNextRCAP, pri++) {
        xRecordClientInfo rci;

        rci.nRanges = pri->nRanges;
        if (client->swapped) {
            swapl(&rci.nRanges);
            RecordSwapRanges(pri->pRanges, pri->nRanges);
        }
        for (i = 0; i < pRCAP->numClients; i++) {
            rci.clientResource = pRCAP->pClientIDs[i];
            if (client->swapped)
                swapl(&rci.clientResource);
            WriteToClient(client, sizeof(xRecordClientInfo), &rci);
            WriteToClient(client, sizeof(xRecordRange) * pri->nRanges,
                          pri->pRanges);
        }
    }
    err = Success;

 bailout:
    for (i = 0; i < nRCAPs; i++) {
        free(pRangeInfo[i].pRanges);
    }
    free(pRangeInfo);
    return err;
}                               /* ProcRecordGetContext */