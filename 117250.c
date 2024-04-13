SProcRecordDisableContext(ClientPtr client)
{
    REQUEST(xRecordDisableContextReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xRecordDisableContextReq);
    swapl(&stuff->context);
    return ProcRecordDisableContext(client);
}                               /* SProcRecordDisableContext */