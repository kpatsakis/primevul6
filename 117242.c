RecordFindClientOnContext(RecordContextPtr pContext,
                          XID clientspec, int *pposition)
{
    RecordClientsAndProtocolPtr pRCAP;

    for (pRCAP = pContext->pListOfRCAP; pRCAP; pRCAP = pRCAP->pNextRCAP) {
        int i;

        for (i = 0; i < pRCAP->numClients; i++) {
            if (pRCAP->pClientIDs[i] == clientspec) {
                if (pposition)
                    *pposition = i;
                return pRCAP;
            }
        }
    }
    return NULL;
}                               /* RecordFindClientOnContext */