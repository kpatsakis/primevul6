RecordAddClientToRCAP(RecordClientsAndProtocolPtr pRCAP, XID clientspec)
{
    if (pRCAP->numClients == pRCAP->sizeClients) {
        if (pRCAP->clientIDsSeparatelyAllocated) {
            XID *pNewIDs =
                reallocarray(pRCAP->pClientIDs,
                             pRCAP->sizeClients + CLIENT_ARRAY_GROWTH_INCREMENT,
                             sizeof(XID));
            if (!pNewIDs)
                return;
            pRCAP->pClientIDs = pNewIDs;
            pRCAP->sizeClients += CLIENT_ARRAY_GROWTH_INCREMENT;
        }
        else {
            XID *pNewIDs =
                xallocarray(pRCAP->sizeClients + CLIENT_ARRAY_GROWTH_INCREMENT,
                            sizeof(XID));
            if (!pNewIDs)
                return;
            memcpy(pNewIDs, pRCAP->pClientIDs, pRCAP->numClients * sizeof(XID));
            pRCAP->pClientIDs = pNewIDs;
            pRCAP->sizeClients += CLIENT_ARRAY_GROWTH_INCREMENT;
            pRCAP->clientIDsSeparatelyAllocated = 1;
        }
    }
    pRCAP->pClientIDs[pRCAP->numClients++] = clientspec;
    if (pRCAP->pContext->pRecordingClient)
        RecordInstallHooks(pRCAP, clientspec);
}                               /* RecordDeleteClientFromRCAP */