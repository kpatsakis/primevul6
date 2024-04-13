SProcRecordFreeContext(ClientPtr client)
{
    REQUEST(xRecordFreeContextReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xRecordFreeContextReq);
    swapl(&stuff->context);
    return ProcRecordFreeContext(client);
}                               /* SProcRecordFreeContext */