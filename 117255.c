RecordAllocRanges(GetContextRangeInfoPtr pri, int nRanges)
{
    int newsize;
    xRecordRange *pNewRange;

#define SZINCR 8

    newsize = max(pri->size + SZINCR, nRanges);
    pNewRange = reallocarray(pri->pRanges, newsize, sizeof(xRecordRange));
    if (!pNewRange)
        return BadAlloc;

    pri->pRanges = pNewRange;
    pri->size = newsize;
    memset(&pri->pRanges[pri->size - SZINCR], 0, SZINCR * sizeof(xRecordRange));
    if (pri->nRanges < nRanges)
        pri->nRanges = nRanges;
    return Success;
}                               /* RecordAllocRanges */