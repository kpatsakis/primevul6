RecordDeleteContext(void *value, XID id)
{
    int i;
    RecordContextPtr pContext = (RecordContextPtr) value;
    RecordClientsAndProtocolPtr pRCAP;

    RecordDisableContext(pContext);

    /*  Remove all the clients from all the RCAPs.
     *  As a result, the RCAPs will be freed.
     */

    while ((pRCAP = pContext->pListOfRCAP)) {
        int numClients = pRCAP->numClients;

        /* when the last client is deleted, the RCAP will go away. */
        while (numClients--) {
            RecordDeleteClientFromRCAP(pRCAP, numClients);
        }
    }

    /* remove context from AllContexts list */

    if (-1 != (i = RecordFindContextOnAllContexts(pContext))) {
        ppAllContexts[i] = ppAllContexts[numContexts - 1];
        if (--numContexts == 0) {
            free(ppAllContexts);
            ppAllContexts = NULL;
        }
    }
    free(pContext);

    return Success;
}                               /* RecordDeleteContext */