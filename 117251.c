RecordABigRequest(RecordContextPtr pContext, ClientPtr client, xReq * stuff)
{
    CARD32 bigLength;
    int bytesLeft;

    /* note: client->req_len has been frobbed by ReadRequestFromClient
     * (os/io.c) to discount the extra 4 bytes taken by the extended length
     * field in a big request.  The actual request length to record is
     * client->req_len + 1 (measured in CARD32s).
     */

    /* record the request header */
    bytesLeft = client->req_len << 2;
    RecordAProtocolElement(pContext, client, XRecordFromClient,
                           (void *) stuff, SIZEOF(xReq), 0, bytesLeft);

    /* reinsert the extended length field that was squished out */
    bigLength = client->req_len + bytes_to_int32(sizeof(bigLength));
    if (client->swapped)
        swapl(&bigLength);
    RecordAProtocolElement(pContext, client, XRecordFromClient,
                           (void *) &bigLength, sizeof(bigLength), 0,
                           /* continuation */ -1);
    bytesLeft -= sizeof(bigLength);

    /* record the rest of the request after the length */
    RecordAProtocolElement(pContext, client, XRecordFromClient,
                           (void *) (stuff + 1), bytesLeft, 0,
                           /* continuation */ -1);
}                               /* RecordABigRequest */