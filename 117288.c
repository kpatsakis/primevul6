ProcRecordQueryVersion(ClientPtr client)
{
    /* REQUEST(xRecordQueryVersionReq); */
    xRecordQueryVersionReply rep = {
        .type = X_Reply,
        .sequenceNumber = client->sequence,
        .length = 0,
        .majorVersion = SERVER_RECORD_MAJOR_VERSION,
        .minorVersion = SERVER_RECORD_MINOR_VERSION
    };

    REQUEST_SIZE_MATCH(xRecordQueryVersionReq);
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swaps(&rep.majorVersion);
        swaps(&rep.minorVersion);
    }
    WriteToClient(client, sizeof(xRecordQueryVersionReply), &rep);
    return Success;
}                               /* ProcRecordQueryVersion */