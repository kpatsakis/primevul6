RecordFindContextOnAllContexts(RecordContextPtr pContext)
{
    int i;

    assert(numContexts >= numEnabledContexts);
    for (i = 0; i < numContexts; i++) {
        if (ppAllContexts[i] == pContext)
            return i;
    }
    return -1;
}                               /* RecordFindContextOnAllContexts */