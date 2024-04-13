RecordConnectionSetupInfo(RecordContextPtr pContext, NewClientInfoRec * pci)
{
    int prefixsize = SIZEOF(xConnSetupPrefix);
    int restsize = pci->prefix->length * 4;

    if (pci->client->swapped) {
        char *pConnSetup = (char *) malloc(prefixsize + restsize);

        if (!pConnSetup)
            return;
        SwapConnSetupPrefix(pci->prefix, (xConnSetupPrefix *) pConnSetup);
        SwapConnSetupInfo((char *) pci->setup,
                          (char *) (pConnSetup + prefixsize));
        RecordAProtocolElement(pContext, pci->client, XRecordClientStarted,
                               (void *) pConnSetup, prefixsize + restsize, 0,
                               0);
        free(pConnSetup);
    }
    else {
        /* don't alloc and copy as in the swapped case; just send the
         * data in two pieces
         */
        RecordAProtocolElement(pContext, pci->client, XRecordClientStarted,
                               (void *) pci->prefix, prefixsize, 0, restsize);
        RecordAProtocolElement(pContext, pci->client, XRecordClientStarted,
                               (void *) pci->setup, restsize, 0,
                               /* continuation */ -1);
    }
}                               /* RecordConnectionSetupInfo */