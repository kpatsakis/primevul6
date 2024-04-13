SProcRecordRegisterClients(ClientPtr client)
{
    REQUEST(xRecordRegisterClientsReq);
    int status;

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xRecordRegisterClientsReq);
    if ((status = SwapCreateRegister(client, (void *) stuff)) != Success)
        return status;
    return ProcRecordRegisterClients(client);
}                               /* SProcRecordRegisterClients */