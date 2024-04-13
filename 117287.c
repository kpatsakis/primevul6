RecordCanonicalizeClientSpecifiers(XID *pClientspecs, int *pNumClientspecs,
                                   XID excludespec)
{
    int i;
    int numClients = *pNumClientspecs;

    /*  first pass strips off the resource index bits, leaving just the
     *  client id bits.  This makes searching for a particular client simpler
     *  (and faster.)
     */
    for (i = 0; i < numClients; i++) {
        XID cs = pClientspecs[i];

        if (cs > XRecordAllClients)
            pClientspecs[i] = CLIENT_BITS(cs);
    }

    for (i = 0; i < numClients; i++) {
        if (pClientspecs[i] == XRecordAllClients || pClientspecs[i] == XRecordCurrentClients) { /* expand All/Current */
            int j, nc;
            XID *pCanon = xallocarray(currentMaxClients + 1, sizeof(XID));

            if (!pCanon)
                return NULL;
            for (nc = 0, j = 1; j < currentMaxClients; j++) {
                ClientPtr client = clients[j];

                if (client != NullClient &&
                    client->clientState == ClientStateRunning &&
                    client->clientAsMask != excludespec) {
                    pCanon[nc++] = client->clientAsMask;
                }
            }
            if (pClientspecs[i] == XRecordAllClients)
                pCanon[nc++] = XRecordFutureClients;
            *pNumClientspecs = nc;
            return pCanon;
        }
        else {                  /* not All or Current */

            int j;

            for (j = i + 1; j < numClients;) {
                if (pClientspecs[i] == pClientspecs[j]) {
                    pClientspecs[j] = pClientspecs[--numClients];
                }
                else
                    j++;
            }
        }
    }                           /* end for each clientspec */
    *pNumClientspecs = numClients;
    return pClientspecs;
}                               /* RecordCanonicalizeClientSpecifiers */