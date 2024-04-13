SProcRecordUnregisterClients(ClientPtr client)
{
    REQUEST(xRecordUnregisterClientsReq);

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xRecordUnregisterClientsReq);
    swapl(&stuff->context);
    swapl(&stuff->nClients);
    SwapRestL(stuff);
    return ProcRecordUnregisterClients(client);
}                               /* SProcRecordUnregisterClients */