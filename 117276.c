ProcRecordFreeContext(ClientPtr client)
{
    RecordContextPtr pContext;

    REQUEST(xRecordFreeContextReq);

    REQUEST_SIZE_MATCH(xRecordFreeContextReq);
    VERIFY_CONTEXT(pContext, stuff->context, client);
    FreeResource(stuff->context, RT_NONE);
    return Success;
}                               /* ProcRecordFreeContext */