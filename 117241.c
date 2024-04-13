SProcRecordDispatch(ClientPtr client)
{
    REQUEST(xReq);

    switch (stuff->data) {
    case X_RecordQueryVersion:
        return SProcRecordQueryVersion(client);
    case X_RecordCreateContext:
        return SProcRecordCreateContext(client);
    case X_RecordRegisterClients:
        return SProcRecordRegisterClients(client);
    case X_RecordUnregisterClients:
        return SProcRecordUnregisterClients(client);
    case X_RecordGetContext:
        return SProcRecordGetContext(client);
    case X_RecordEnableContext:
        return SProcRecordEnableContext(client);
    case X_RecordDisableContext:
        return SProcRecordDisableContext(client);
    case X_RecordFreeContext:
        return SProcRecordFreeContext(client);
    default:
        return BadRequest;
    }
}                               /* SProcRecordDispatch */