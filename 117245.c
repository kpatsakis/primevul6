RecordExtensionInit(void)
{
    ExtensionEntry *extentry;

    RTContext = CreateNewResourceType(RecordDeleteContext, "RecordContext");
    if (!RTContext)
        return;

    if (!dixRegisterPrivateKey(RecordClientPrivateKey, PRIVATE_CLIENT, 0))
        return;

    ppAllContexts = NULL;
    numContexts = numEnabledContexts = numEnabledRCAPs = 0;

    if (!AddCallback(&ClientStateCallback, RecordAClientStateChange, NULL))
        return;

    extentry = AddExtension(RECORD_NAME, RecordNumEvents, RecordNumErrors,
                            ProcRecordDispatch, SProcRecordDispatch,
                            RecordCloseDown, StandardMinorOpcode);
    if (!extentry) {
        DeleteCallback(&ClientStateCallback, RecordAClientStateChange, NULL);
        return;
    }
    SetResourceTypeErrorValue(RTContext,
                              extentry->errorBase + XRecordBadContext);

}                               /* RecordExtensionInit */