RecordSanityCheckClientSpecifiers(ClientPtr client, XID *clientspecs,
                                  int nspecs, XID errorspec)
{
    int i;
    int clientIndex;
    int rc;
    void *value;

    for (i = 0; i < nspecs; i++) {
        if (clientspecs[i] == XRecordCurrentClients ||
            clientspecs[i] == XRecordFutureClients ||
            clientspecs[i] == XRecordAllClients)
            continue;
        if (errorspec && (CLIENT_BITS(clientspecs[i]) == errorspec))
            return BadMatch;
        clientIndex = CLIENT_ID(clientspecs[i]);
        if (clientIndex && clients[clientIndex] &&
            clients[clientIndex]->clientState == ClientStateRunning) {
            if (clientspecs[i] == clients[clientIndex]->clientAsMask)
                continue;
            rc = dixLookupResourceByClass(&value, clientspecs[i], RC_ANY,
                                          client, DixGetAttrAccess);
            if (rc != Success)
                return rc;
        }
        else
            return BadMatch;
    }
    return Success;
}                               /* RecordSanityCheckClientSpecifiers */