RecordARequest(ClientPtr client)
{
    RecordContextPtr pContext;
    RecordClientsAndProtocolPtr pRCAP;
    int i;
    RecordClientPrivatePtr pClientPriv;

    REQUEST(xReq);
    int majorop;

    majorop = stuff->reqType;
    for (i = 0; i < numEnabledContexts; i++) {
        pContext = ppAllContexts[i];
        pRCAP = RecordFindClientOnContext(pContext, client->clientAsMask, NULL);
        if (pRCAP && pRCAP->pRequestMajorOpSet &&
            RecordIsMemberOfSet(pRCAP->pRequestMajorOpSet, majorop)) {
            if (majorop <= 127) {       /* core request */

                if (stuff->length == 0)
                    RecordABigRequest(pContext, client, stuff);
                else
                    RecordAProtocolElement(pContext, client, XRecordFromClient,
                                           (void *) stuff,
                                           client->req_len << 2, 0, 0);
            }
            else {              /* extension, check minor opcode */

                int minorop = client->minorOp;
                int numMinOpInfo;
                RecordMinorOpPtr pMinorOpInfo = pRCAP->pRequestMinOpInfo;

                assert(pMinorOpInfo);
                numMinOpInfo = pMinorOpInfo->count;
                pMinorOpInfo++;
                assert(numMinOpInfo);
                for (; numMinOpInfo; numMinOpInfo--, pMinorOpInfo++) {
                    if (majorop >= pMinorOpInfo->major.first &&
                        majorop <= pMinorOpInfo->major.last &&
                        RecordIsMemberOfSet(pMinorOpInfo->major.pMinOpSet,
                                            minorop)) {
                        if (stuff->length == 0)
                            RecordABigRequest(pContext, client, stuff);
                        else
                            RecordAProtocolElement(pContext, client,
                                                   XRecordFromClient,
                                                   (void *) stuff,
                                                   client->req_len << 2, 0, 0);
                        break;
                    }
                }               /* end for each minor op info */
            }                   /* end extension request */
        }                       /* end this RCAP wants this major opcode */
    }                           /* end for each context */
    pClientPriv = RecordClientPrivate(client);
    assert(pClientPriv);
    return (*pClientPriv->originalVector[majorop]) (client);
}                               /* RecordARequest */