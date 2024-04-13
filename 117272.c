SProcRecordCreateContext(ClientPtr client)
{
    REQUEST(xRecordCreateContextReq);
    int status;

    swaps(&stuff->length);
    REQUEST_AT_LEAST_SIZE(xRecordCreateContextReq);
    if ((status = SwapCreateRegister(client, (void *) stuff)) != Success)
        return status;
    return ProcRecordCreateContext(client);
}                               /* SProcRecordCreateContext */