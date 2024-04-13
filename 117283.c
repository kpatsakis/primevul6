ProcRecordCreateContext(ClientPtr client)
{
    REQUEST(xRecordCreateContextReq);
    RecordContextPtr pContext;
    RecordContextPtr *ppNewAllContexts = NULL;
    int err = BadAlloc;

    REQUEST_AT_LEAST_SIZE(xRecordCreateContextReq);
    LEGAL_NEW_RESOURCE(stuff->context, client);

    pContext = (RecordContextPtr) malloc(sizeof(RecordContextRec));
    if (!pContext)
        goto bailout;

    /* make sure there is room in ppAllContexts to store the new context */

    ppNewAllContexts =
        reallocarray(ppAllContexts, numContexts + 1, sizeof(RecordContextPtr));
    if (!ppNewAllContexts)
        goto bailout;
    ppAllContexts = ppNewAllContexts;

    pContext->id = stuff->context;
    pContext->pRecordingClient = NULL;
    pContext->pListOfRCAP = NULL;
    pContext->elemHeaders = 0;
    pContext->bufCategory = 0;
    pContext->numBufBytes = 0;
    pContext->pBufClient = NULL;
    pContext->continuedReply = 0;
    pContext->inFlush = 0;

    err = RecordRegisterClients(pContext, client,
                                (xRecordRegisterClientsReq *) stuff);
    if (err != Success)
        goto bailout;

    if (AddResource(pContext->id, RTContext, pContext)) {
        ppAllContexts[numContexts++] = pContext;
        return Success;
    }
    else {
        return BadAlloc;
    }
 bailout:
    free(pContext);
    return err;
}                               /* ProcRecordCreateContext */