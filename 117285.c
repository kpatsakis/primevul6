RecordCloseDown(ExtensionEntry * extEntry)
{
    DeleteCallback(&ClientStateCallback, RecordAClientStateChange, NULL);
}                               /* RecordCloseDown */