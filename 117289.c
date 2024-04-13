ProcRecordEnableContext(ClientPtr client)
{
    RecordContextPtr pContext;

    REQUEST(xRecordEnableContextReq);
    int i;
    RecordClientsAndProtocolPtr pRCAP;

    REQUEST_SIZE_MATCH(xRecordGetContextReq);
    VERIFY_CONTEXT(pContext, stuff->context, client);
    if (pContext->pRecordingClient)
        return BadMatch;        /* already enabled */

    /* install record hooks for each RCAP */

    for (pRCAP = pContext->pListOfRCAP; pRCAP; pRCAP = pRCAP->pNextRCAP) {
        int err = RecordInstallHooks(pRCAP, 0);

        if (err != Success) {   /* undo the previous installs */
            RecordClientsAndProtocolPtr pUninstallRCAP;

            for (pUninstallRCAP = pContext->pListOfRCAP;
                 pUninstallRCAP != pRCAP;
                 pUninstallRCAP = pUninstallRCAP->pNextRCAP) {
                RecordUninstallHooks(pUninstallRCAP, 0);
            }
            return err;
        }
    }

    /* Disallow further request processing on this connection until
     * the context is disabled.
     */
    IgnoreClient(client);
    pContext->pRecordingClient = client;

    /* Don't allow the data connection to record itself; unregister it. */
    RecordDeleteClientFromContext(pContext,
                                  pContext->pRecordingClient->clientAsMask);

    /* move the newly enabled context to the front part of ppAllContexts,
     * where all the enabled contexts are
     */
    i = RecordFindContextOnAllContexts(pContext);
    assert(i >= numEnabledContexts);
    if (i != numEnabledContexts) {
        ppAllContexts[i] = ppAllContexts[numEnabledContexts];
        ppAllContexts[numEnabledContexts] = pContext;
    }

    ++numEnabledContexts;
    assert(numEnabledContexts > 0);

    /* send StartOfData */
    RecordAProtocolElement(pContext, NULL, XRecordStartOfData, NULL, 0, 0, 0);
    RecordFlushReplyBuffer(pContext, NULL, 0, NULL, 0);
    return Success;
}                               /* ProcRecordEnableContext */