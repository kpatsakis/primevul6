ProcRecordDispatch(ClientPtr client)
{
    REQUEST(xReq);

    switch (stuff->data) {
    case X_RecordQueryVersion:
        return ProcRecordQueryVersion(client);
    case X_RecordCreateContext:
        return ProcRecordCreateContext(client);
    case X_RecordRegisterClients:
        return ProcRecordRegisterClients(client);
    case X_RecordUnregisterClients:
        return ProcRecordUnregisterClients(client);
    case X_RecordGetContext:
        return ProcRecordGetContext(client);
    case X_RecordEnableContext:
        return ProcRecordEnableContext(client);
    case X_RecordDisableContext:
        return ProcRecordDisableContext(client);
    case X_RecordFreeContext:
        return ProcRecordFreeContext(client);
    default:
        return BadRequest;
    }
}                               /* ProcRecordDispatch */