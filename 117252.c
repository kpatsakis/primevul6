RecordDisableContext(RecordContextPtr pContext)
{
    RecordClientsAndProtocolPtr pRCAP;
    int i;

    if (!pContext->pRecordingClient)
        return;
    if (!pContext->pRecordingClient->clientGone) {
        RecordAProtocolElement(pContext, NULL, XRecordEndOfData, NULL, 0, 0, 0);
        RecordFlushReplyBuffer(pContext, NULL, 0, NULL, 0);
    }
    /* Re-enable request processing on this connection. */
    AttendClient(pContext->pRecordingClient);

    for (pRCAP = pContext->pListOfRCAP; pRCAP; pRCAP = pRCAP->pNextRCAP) {
        RecordUninstallHooks(pRCAP, 0);
    }

    pContext->pRecordingClient = NULL;

    /* move the newly disabled context to the rear part of ppAllContexts,
     * where all the disabled contexts are
     */
    i = RecordFindContextOnAllContexts(pContext);
    assert(i != -1);
    assert(i < numEnabledContexts);
    if (i != (numEnabledContexts - 1)) {
        ppAllContexts[i] = ppAllContexts[numEnabledContexts - 1];
        ppAllContexts[numEnabledContexts - 1] = pContext;
    }
    --numEnabledContexts;
    assert(numEnabledContexts >= 0);
}                               /* RecordDisableContext */