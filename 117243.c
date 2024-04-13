ProcRecordRegisterClients(ClientPtr client)
{
    RecordContextPtr pContext;

    REQUEST(xRecordRegisterClientsReq);

    REQUEST_AT_LEAST_SIZE(xRecordRegisterClientsReq);
    VERIFY_CONTEXT(pContext, stuff->context, client);

    return RecordRegisterClients(pContext, client, stuff);
}                               /* ProcRecordRegisterClients */