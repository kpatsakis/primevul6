SProcRecordEnableContext(ClientPtr client)
{
    REQUEST(xRecordEnableContextReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xRecordEnableContextReq);
    swapl(&stuff->context);
    return ProcRecordEnableContext(client);
}                               /* SProcRecordEnableContext */