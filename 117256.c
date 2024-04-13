RecordFlushAllContexts(CallbackListPtr *pcbl,
                       void *nulldata, void *calldata)
{
    int eci;                    /* enabled context index */
    RecordContextPtr pContext;

    for (eci = 0; eci < numEnabledContexts; eci++) {
        pContext = ppAllContexts[eci];

        /* In most cases we leave it to RecordFlushReplyBuffer to make
         * this check, but this function could be called very often, so we
         * check before calling hoping to save the function call cost
         * most of the time.
         */
        if (pContext->numBufBytes)
            RecordFlushReplyBuffer(ppAllContexts[eci], NULL, 0, NULL, 0);
    }
}                               /* RecordFlushAllContexts */