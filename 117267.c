SProcRecordQueryVersion(ClientPtr client)
{
    REQUEST(xRecordQueryVersionReq);

    swaps(&stuff->length);
    REQUEST_SIZE_MATCH(xRecordQueryVersionReq);
    swaps(&stuff->majorVersion);
    swaps(&stuff->minorVersion);
    return ProcRecordQueryVersion(client);
}                               /* SProcRecordQueryVersion */