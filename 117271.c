RecordUninstallHooks(RecordClientsAndProtocolPtr pRCAP, XID oneclient)
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
                ClientPtr pClient = clients[CLIENT_ID(client)];
                int c;
                Bool otherRCAPwantsProcVector = FALSE;
                RecordClientPrivatePtr pClientPriv = NULL;

                assert(pClient);
                pClientPriv = RecordClientPrivate(pClient);
                assert(pClientPriv);
                memcpy(pClientPriv->recordVector, pClientPriv->originalVector,
                       sizeof(pClientPriv->recordVector));

                for (c = 0; c < numEnabledContexts; c++) {
                    RecordClientsAndProtocolPtr pOtherRCAP;
                    RecordContextPtr pContext = ppAllContexts[c];

                    if (pContext == pRCAP->pContext)
                        continue;
                    pOtherRCAP = RecordFindClientOnContext(pContext, client,
                                                           NULL);
                    if (pOtherRCAP && pOtherRCAP->pRequestMajorOpSet) {
                        RecordSetIteratePtr pIter = NULL;
                        RecordSetInterval interval;

                        otherRCAPwantsProcVector = TRUE;
                        while ((pIter =
                                RecordIterateSet(pOtherRCAP->pRequestMajorOpSet,
                                                 pIter, &interval))) {
                            unsigned int j;

                            for (j = interval.first; j <= interval.last; j++)
                                pClient->requestVector[j] = RecordARequest;
                        }
                    }
                }
                if (!otherRCAPwantsProcVector) {        /* nobody needs it, so free it */
                    pClient->requestVector = pClientPriv->originalVector;
                    dixSetPrivate(&pClient->devPrivates,
                                  RecordClientPrivateKey, NULL);
                    free(pClientPriv);
                }
            }                   /* end if this RCAP specifies any requests */
        }                       /* end if not future clients */
        if (oneclient)
            client = 0;
        else
            client = (i < pRCAP->numClients) ? pRCAP->pClientIDs[i++] : 0;
    }

    assert(numEnabledRCAPs >= 1);
    if (!oneclient && --numEnabledRCAPs == 0) { /* we're disabling the last context */
        DeleteCallback(&EventCallback, RecordADeliveredEventOrError, NULL);
        DeleteCallback(&DeviceEventCallback, RecordADeviceEvent, NULL);
        DeleteCallback(&ReplyCallback, RecordAReply, NULL);
        DeleteCallback(&FlushCallback, RecordFlushAllContexts, NULL);
        /* Alternate context flushing scheme: delete the line above
         * and call RemoveBlockAndWakeupHandlers here passing
         * RecordFlushAllContexts.  Is this any better?
         */
        /* Having deleted the callback, call it one last time. -gildea */
        RecordFlushAllContexts(&FlushCallback, NULL, NULL);
    }
}                               /* RecordUninstallHooks */