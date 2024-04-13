RecordAClientStateChange(CallbackListPtr *pcbl, void *nulldata,
                         void *calldata)
{
    NewClientInfoRec *pci = (NewClientInfoRec *) calldata;
    int i;
    ClientPtr pClient = pci->client;
    RecordContextPtr *ppAllContextsCopy = NULL;
    int numContextsCopy = 0;

    switch (pClient->clientState) {
    case ClientStateRunning:   /* new client */
        for (i = 0; i < numContexts; i++) {
            RecordClientsAndProtocolPtr pRCAP;
            RecordContextPtr pContext = ppAllContexts[i];

            if ((pRCAP = RecordFindClientOnContext(pContext,
                                                   XRecordFutureClients, NULL)))
            {
                RecordAddClientToRCAP(pRCAP, pClient->clientAsMask);
                if (pContext->pRecordingClient && pRCAP->clientStarted)
                    RecordConnectionSetupInfo(pContext, pci);
            }
        }
        break;

    case ClientStateGone:
    case ClientStateRetained:  /* client disconnected */

        /* RecordDisableContext modifies contents of ppAllContexts. */
        if (!(numContextsCopy = numContexts))
            break;
        ppAllContextsCopy = xallocarray(numContextsCopy,
                                        sizeof(RecordContextPtr));
        assert(ppAllContextsCopy);
        memcpy(ppAllContextsCopy, ppAllContexts,
               numContextsCopy * sizeof(RecordContextPtr));

        for (i = 0; i < numContextsCopy; i++) {
            RecordClientsAndProtocolPtr pRCAP;
            RecordContextPtr pContext = ppAllContextsCopy[i];
            int pos;

            if (pContext->pRecordingClient == pClient)
                RecordDisableContext(pContext);
            if ((pRCAP = RecordFindClientOnContext(pContext,
                                                   pClient->clientAsMask,
                                                   &pos))) {
                if (pContext->pRecordingClient && pRCAP->clientDied)
                    RecordAProtocolElement(pContext, pClient,
                                           XRecordClientDied, NULL, 0, 0, 0);
                RecordDeleteClientFromRCAP(pRCAP, pos);
            }
        }

        free(ppAllContextsCopy);
        break;

    default:
        break;
    }                           /* end switch on client state */
}                               /* RecordAClientStateChange */