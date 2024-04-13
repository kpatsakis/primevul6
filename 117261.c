SProcRecordGetContext(ClientPtr client)
{
    REQUEST(xRecordGetContextReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xRecordGetContextReq);
    swapl(&stuff->context);
    return ProcRecordGetContext(client);
}                               /* SProcRecordGetContext */