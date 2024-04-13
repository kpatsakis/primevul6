RecordADeliveredEventOrError(CallbackListPtr *pcbl, void *nulldata,
                             void *calldata)
{
    EventInfoRec *pei = (EventInfoRec *) calldata;
    RecordContextPtr pContext;
    RecordClientsAndProtocolPtr pRCAP;
    int eci;                    /* enabled context index */
    ClientPtr pClient = pei->client;

    for (eci = 0; eci < numEnabledContexts; eci++) {
        pContext = ppAllContexts[eci];
        pRCAP = RecordFindClientOnContext(pContext, pClient->clientAsMask,
                                          NULL);
        if (pRCAP && (pRCAP->pDeliveredEventSet || pRCAP->pErrorSet)) {
            int ev;             /* event index */
            xEvent *pev = pei->events;

            for (ev = 0; ev < pei->count; ev++, pev++) {
                int recordit = 0;

                if (pRCAP->pErrorSet) {
                    recordit = RecordIsMemberOfSet(pRCAP->pErrorSet,
                                                   ((xError *) (pev))->
                                                   errorCode);
                }
                else if (pRCAP->pDeliveredEventSet) {
                    recordit = RecordIsMemberOfSet(pRCAP->pDeliveredEventSet,
                                                   pev->u.u.type & 0177);
                }
                if (recordit) {
                    xEvent swappedEvent;
                    xEvent *pEvToRecord = pev;

                    if (pClient->swapped) {
                        (*EventSwapVector[pev->u.u.type & 0177])
                            (pev, &swappedEvent);
                        pEvToRecord = &swappedEvent;

                    }
                    RecordAProtocolElement(pContext, pClient,
                                           XRecordFromServer, pEvToRecord,
                                           SIZEOF(xEvent), 0, 0);
                }
            }                   /* end for each event */
        }                       /* end this client is on this context */
    }                           /* end for each enabled context */
}                               /* RecordADeliveredEventOrError */