RecordFlushReplyBuffer(RecordContextPtr pContext,
                       void *data1, int len1, void *data2, int len2)
{
    if (!pContext->pRecordingClient || pContext->pRecordingClient->clientGone ||
        pContext->inFlush)
        return;
    ++pContext->inFlush;
    if (pContext->numBufBytes)
        WriteToClient(pContext->pRecordingClient, pContext->numBufBytes,
                      pContext->replyBuffer);
    pContext->numBufBytes = 0;
    if (len1)
        WriteToClient(pContext->pRecordingClient, len1, data1);
    if (len2)
        WriteToClient(pContext->pRecordingClient, len2, data2);
    --pContext->inFlush;
}                               /* RecordFlushReplyBuffer */