ProcRecordDisableContext(ClientPtr client)
{
    RecordContextPtr pContext;

    REQUEST(xRecordDisableContextReq);

    REQUEST_SIZE_MATCH(xRecordDisableContextReq);
    VERIFY_CONTEXT(pContext, stuff->context, client);
    RecordDisableContext(pContext);
    return Success;
}                               /* ProcRecordDisableContext */