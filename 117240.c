ProcRecordUnregisterClients(ClientPtr client)
{
    RecordContextPtr pContext;
    int err;

    REQUEST(xRecordUnregisterClientsReq);
    XID *pCanonClients;
    int nClients;
    int i;

    REQUEST_AT_LEAST_SIZE(xRecordUnregisterClientsReq);
    if (INT_MAX / 4 < stuff->nClients ||
        (client->req_len << 2) - SIZEOF(xRecordUnregisterClientsReq) !=
        4 * stuff->nClients)
        return BadLength;
    VERIFY_CONTEXT(pContext, stuff->context, client);
    err = RecordSanityCheckClientSpecifiers(client, (XID *) &stuff[1],
                                            stuff->nClients, 0);
    if (err != Success)
        return err;

    nClients = stuff->nClients;
    pCanonClients = RecordCanonicalizeClientSpecifiers((XID *) &stuff[1],
                                                       &nClients, 0);
    if (!pCanonClients)
        return BadAlloc;

    for (i = 0; i < nClients; i++) {
        RecordDeleteClientFromContext(pContext, pCanonClients[i]);
    }
    if (pCanonClients != (XID *) &stuff[1])
        free(pCanonClients);
    return Success;
}                               /* ProcRecordUnregisterClients */