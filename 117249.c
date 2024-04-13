RecordRegisterClients(RecordContextPtr pContext, ClientPtr client,
                      xRecordRegisterClientsReq * stuff)
{
    int err;
    int i;
    SetInfoPtr si;
    int maxSets;
    int nExtReqSets = 0;
    int nExtRepSets = 0;
    int extReqSetsOffset = 0;
    int extRepSetsOffset = 0;
    SetInfoPtr pExtReqSets, pExtRepSets;
    int clientListOffset;
    XID *pCanonClients;
    int clientStarted = 0, clientDied = 0;
    xRecordRange *pRanges, rr;
    int nClients;
    int sizeClients;
    int totRCAPsize;
    RecordClientsAndProtocolPtr pRCAP;
    int pad;
    XID recordingClient;

    /* do all sanity checking up front */

    err = RecordSanityCheckRegisterClients(pContext, client, stuff);
    if (err != Success)
        return err;

    /* if element headers changed, flush buffer */

    if (pContext->elemHeaders != stuff->elementHeader) {
        RecordFlushReplyBuffer(pContext, NULL, 0, NULL, 0);
        pContext->elemHeaders = stuff->elementHeader;
    }

    nClients = stuff->nClients;
    if (!nClients)
        /* if empty clients list, we're done. */
        return Success;

    recordingClient = pContext->pRecordingClient ?
        pContext->pRecordingClient->clientAsMask : 0;
    pCanonClients = RecordCanonicalizeClientSpecifiers((XID *) &stuff[1],
                                                       &nClients,
                                                       recordingClient);
    if (!pCanonClients)
        return BadAlloc;

    /* We may have to create as many as one set for each "predefined"
     * protocol types, plus one per range for extension requests, plus one per
     * range for extension replies.
     */
    maxSets = PREDEFSETS + 2 * stuff->nRanges;
    si = xallocarray(maxSets, sizeof(SetInfoRec));
    if (!si) {
        err = BadAlloc;
        goto bailout;
    }
    memset(si, 0, sizeof(SetInfoRec) * maxSets);

    /* theoretically you must do this because NULL may not be all-bits-zero */
    for (i = 0; i < maxSets; i++)
        si[i].intervals = NULL;

    pExtReqSets = si + PREDEFSETS;
    pExtRepSets = pExtReqSets + stuff->nRanges;

    pRanges = (xRecordRange *) (((XID *) &stuff[1]) + stuff->nClients);

    err = RecordConvertRangesToIntervals(&si[REQ], pRanges, stuff->nRanges,
                                         offset_of(rr, coreRequestsFirst), NULL,
                                         NULL);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[REQ], pRanges, stuff->nRanges,
                                         offset_of(rr, extRequestsMajorFirst),
                                         pExtReqSets, &nExtReqSets);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[REP], pRanges, stuff->nRanges,
                                         offset_of(rr, coreRepliesFirst), NULL,
                                         NULL);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[REP], pRanges, stuff->nRanges,
                                         offset_of(rr, extRepliesMajorFirst),
                                         pExtRepSets, &nExtRepSets);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[ERR], pRanges, stuff->nRanges,
                                         offset_of(rr, errorsFirst), NULL,
                                         NULL);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[DLEV], pRanges, stuff->nRanges,
                                         offset_of(rr, deliveredEventsFirst),
                                         NULL, NULL);
    if (err != Success)
        goto bailout;

    err = RecordConvertRangesToIntervals(&si[DEV], pRanges, stuff->nRanges,
                                         offset_of(rr, deviceEventsFirst), NULL,
                                         NULL);
    if (err != Success)
        goto bailout;

    /* collect client-started and client-died */

    for (i = 0; i < stuff->nRanges; i++) {
        if (pRanges[i].clientStarted)
            clientStarted = TRUE;
        if (pRanges[i].clientDied)
            clientDied = TRUE;
    }

    /*  We now have all the information collected to create all the sets,
     * and we can compute the total memory required for the RCAP.
     */

    totRCAPsize = sizeof(RecordClientsAndProtocolRec);

    /* leave a little room to grow before forcing a separate allocation */
    sizeClients = nClients + CLIENT_ARRAY_GROWTH_INCREMENT;
    pad = RecordPadAlign(totRCAPsize, sizeof(XID));
    clientListOffset = totRCAPsize + pad;
    totRCAPsize += pad + sizeClients * sizeof(XID);

    if (nExtReqSets) {
        pad = RecordPadAlign(totRCAPsize, sizeof(RecordSetPtr));
        extReqSetsOffset = totRCAPsize + pad;
        totRCAPsize += pad + (nExtReqSets + 1) * sizeof(RecordMinorOpRec);
    }
    if (nExtRepSets) {
        pad = RecordPadAlign(totRCAPsize, sizeof(RecordSetPtr));
        extRepSetsOffset = totRCAPsize + pad;
        totRCAPsize += pad + (nExtRepSets + 1) * sizeof(RecordMinorOpRec);
    }

    for (i = 0; i < maxSets; i++) {
        if (si[i].nintervals) {
            si[i].size =
                RecordSetMemoryRequirements(si[i].intervals, si[i].nintervals,
                                            &si[i].align);
            pad = RecordPadAlign(totRCAPsize, si[i].align);
            si[i].offset = pad + totRCAPsize;
            totRCAPsize += pad + si[i].size;
        }
    }

    /* allocate memory for the whole RCAP */

    pRCAP = (RecordClientsAndProtocolPtr) malloc(totRCAPsize);
    if (!pRCAP) {
        err = BadAlloc;
        goto bailout;
    }

    /* fill in the RCAP */

    pRCAP->pContext = pContext;
    pRCAP->pClientIDs = (XID *) ((char *) pRCAP + clientListOffset);
    pRCAP->numClients = nClients;
    pRCAP->sizeClients = sizeClients;
    pRCAP->clientIDsSeparatelyAllocated = 0;
    for (i = 0; i < nClients; i++) {
        RecordDeleteClientFromContext(pContext, pCanonClients[i]);
        pRCAP->pClientIDs[i] = pCanonClients[i];
    }

    /* create all the sets */

    if (si[REQ].intervals) {
        pRCAP->pRequestMajorOpSet =
            RecordCreateSet(si[REQ].intervals, si[REQ].nintervals,
                            (RecordSetPtr) ((char *) pRCAP + si[REQ].offset),
                            si[REQ].size);
    }
    else
        pRCAP->pRequestMajorOpSet = NULL;

    if (si[REP].intervals) {
        pRCAP->pReplyMajorOpSet =
            RecordCreateSet(si[REP].intervals, si[REP].nintervals,
                            (RecordSetPtr) ((char *) pRCAP + si[REP].offset),
                            si[REP].size);
    }
    else
        pRCAP->pReplyMajorOpSet = NULL;

    if (si[ERR].intervals) {
        pRCAP->pErrorSet =
            RecordCreateSet(si[ERR].intervals, si[ERR].nintervals,
                            (RecordSetPtr) ((char *) pRCAP + si[ERR].offset),
                            si[ERR].size);
    }
    else
        pRCAP->pErrorSet = NULL;

    if (si[DEV].intervals) {
        pRCAP->pDeviceEventSet =
            RecordCreateSet(si[DEV].intervals, si[DEV].nintervals,
                            (RecordSetPtr) ((char *) pRCAP + si[DEV].offset),
                            si[DEV].size);
    }
    else
        pRCAP->pDeviceEventSet = NULL;

    if (si[DLEV].intervals) {
        pRCAP->pDeliveredEventSet =
            RecordCreateSet(si[DLEV].intervals, si[DLEV].nintervals,
                            (RecordSetPtr) ((char *) pRCAP + si[DLEV].offset),
                            si[DLEV].size);
    }
    else
        pRCAP->pDeliveredEventSet = NULL;

    if (nExtReqSets) {
        pRCAP->pRequestMinOpInfo = (RecordMinorOpPtr)
            ((char *) pRCAP + extReqSetsOffset);
        pRCAP->pRequestMinOpInfo[0].count = nExtReqSets;
        for (i = 0; i < nExtReqSets; i++, pExtReqSets++) {
            pRCAP->pRequestMinOpInfo[i + 1].major.first = pExtReqSets->first;
            pRCAP->pRequestMinOpInfo[i + 1].major.last = pExtReqSets->last;
            pRCAP->pRequestMinOpInfo[i + 1].major.pMinOpSet =
                RecordCreateSet(pExtReqSets->intervals,
                                pExtReqSets->nintervals,
                                (RecordSetPtr) ((char *) pRCAP +
                                                pExtReqSets->offset),
                                pExtReqSets->size);
        }
    }
    else
        pRCAP->pRequestMinOpInfo = NULL;

    if (nExtRepSets) {
        pRCAP->pReplyMinOpInfo = (RecordMinorOpPtr)
            ((char *) pRCAP + extRepSetsOffset);
        pRCAP->pReplyMinOpInfo[0].count = nExtRepSets;
        for (i = 0; i < nExtRepSets; i++, pExtRepSets++) {
            pRCAP->pReplyMinOpInfo[i + 1].major.first = pExtRepSets->first;
            pRCAP->pReplyMinOpInfo[i + 1].major.last = pExtRepSets->last;
            pRCAP->pReplyMinOpInfo[i + 1].major.pMinOpSet =
                RecordCreateSet(pExtRepSets->intervals,
                                pExtRepSets->nintervals,
                                (RecordSetPtr) ((char *) pRCAP +
                                                pExtRepSets->offset),
                                pExtRepSets->size);
        }
    }
    else
        pRCAP->pReplyMinOpInfo = NULL;

    pRCAP->clientStarted = clientStarted;
    pRCAP->clientDied = clientDied;

    /* link the RCAP onto the context */

    pRCAP->pNextRCAP = pContext->pListOfRCAP;
    pContext->pListOfRCAP = pRCAP;

    if (pContext->pRecordingClient)     /* context enabled */
        RecordInstallHooks(pRCAP, 0);

 bailout:
    if (si) {
        for (i = 0; i < maxSets; i++)
            free(si[i].intervals);
        free(si);
    }
    if (pCanonClients && pCanonClients != (XID *) &stuff[1])
        free(pCanonClients);
    return err;
}                               /* RecordRegisterClients */