RecordAReply(CallbackListPtr *pcbl, void *nulldata, void *calldata)
{
    RecordContextPtr pContext;
    RecordClientsAndProtocolPtr pRCAP;
    int eci;
    ReplyInfoRec *pri = (ReplyInfoRec *) calldata;
    ClientPtr client = pri->client;

    for (eci = 0; eci < numEnabledContexts; eci++) {
        pContext = ppAllContexts[eci];
        pRCAP = RecordFindClientOnContext(pContext, client->clientAsMask, NULL);
        if (pRCAP) {
            int majorop = client->majorOp;

            if (pContext->continuedReply) {
                RecordAProtocolElement(pContext, client, XRecordFromServer,
                                       (void *) pri->replyData,
                                       pri->dataLenBytes, pri->padBytes,
                                       /* continuation */ -1);
                if (!pri->bytesRemaining)
                    pContext->continuedReply = 0;
            }
            else if (pri->startOfReply && pRCAP->pReplyMajorOpSet &&
                     RecordIsMemberOfSet(pRCAP->pReplyMajorOpSet, majorop)) {
                if (majorop <= 127) {   /* core reply */
                    RecordAProtocolElement(pContext, client, XRecordFromServer,
                                           (void *) pri->replyData,
                                           pri->dataLenBytes, 0,
                                           pri->bytesRemaining);
                    if (pri->bytesRemaining)
                        pContext->continuedReply = 1;
                }
                else {          /* extension, check minor opcode */

                    int minorop = client->minorOp;
                    int numMinOpInfo;
                    RecordMinorOpPtr pMinorOpInfo = pRCAP->pReplyMinOpInfo;

                    assert(pMinorOpInfo);
                    numMinOpInfo = pMinorOpInfo->count;
                    pMinorOpInfo++;
                    assert(numMinOpInfo);
                    for (; numMinOpInfo; numMinOpInfo--, pMinorOpInfo++) {
                        if (majorop >= pMinorOpInfo->major.first &&
                            majorop <= pMinorOpInfo->major.last &&
                            RecordIsMemberOfSet(pMinorOpInfo->major.pMinOpSet,
                                                minorop)) {
                            RecordAProtocolElement(pContext, client,
                                                   XRecordFromServer,
                                                   (void *) pri->replyData,
                                                   pri->dataLenBytes, 0,
                                                   pri->bytesRemaining);
                            if (pri->bytesRemaining)
                                pContext->continuedReply = 1;
                            break;
                        }
                    }           /* end for each minor op info */
                }               /* end extension reply */
            }                   /* end continued reply vs. start of reply */
        }                       /* end client is registered on this context */
    }                           /* end for each context */
}                               /* RecordAReply */