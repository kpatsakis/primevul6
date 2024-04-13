RecordInstallHooks(RecordClientsAndProtocolPtr pRCAP, XID oneclient)
{
    int i = 0;
    XID client;

    if (oneclient)
        client = oneclient;
    else
        client = pRCAP->numClients ? pRCAP->pClientIDs[i++] : 0;

    while (client) {
        if (client != XRecordFutureClients) {
            if (pRCAP->pRequestMajorOpSet) {
                RecordSetIteratePtr pIter = NULL;
                RecordSetInterval interval;
                ClientPtr pClient = clients[CLIENT_ID(client)];

                if (pClient && !RecordClientPrivate(pClient)) {
                    RecordClientPrivatePtr pClientPriv;

                    /* no Record proc vector; allocate one */
                    pClientPriv = (RecordClientPrivatePtr)
                        malloc(sizeof(RecordClientPrivateRec));
                    if (!pClientPriv)
                        return BadAlloc;
                    /* copy old proc vector to new */
                    memcpy(pClientPriv->recordVector, pClient->requestVector,
                           sizeof(pClientPriv->recordVector));
                    pClientPriv->originalVector = pClient->requestVector;
                    dixSetPrivate(&pClient->devPrivates,
                                  RecordClientPrivateKey, pClientPriv);
                    pClient->requestVector = pClientPriv->recordVector;
                }
                while ((pIter = RecordIterateSet(pRCAP->pRequestMajorOpSet,
                                                 pIter, &interval))) {
                    unsigned int j;

                    for (j = interval.first; j <= interval.last; j++)
                        pClient->requestVector[j] = RecordARequest;
                }
            }
        }
        if (oneclient)
            client = 0;
        else
            client = (i < pRCAP->numClients) ? pRCAP->pClientIDs[i++] : 0;
    }

    assert(numEnabledRCAPs >= 0);
    if (!oneclient && ++numEnabledRCAPs == 1) { /* we're enabling the first context */
        if (!AddCallback(&EventCallback, RecordADeliveredEventOrError, NULL))
            return BadAlloc;
        if (!AddCallback(&DeviceEventCallback, RecordADeviceEvent, NULL))
            return BadAlloc;
        if (!AddCallback(&ReplyCallback, RecordAReply, NULL))
            return BadAlloc;
        if (!AddCallback(&FlushCallback, RecordFlushAllContexts, NULL))
            return BadAlloc;
        /* Alternate context flushing scheme: delete the line above
         * and call RegisterBlockAndWakeupHandlers here passing
         * RecordFlushAllContexts.  Is this any better?
         */
    }
    return Success;
}                               /* RecordInstallHooks */