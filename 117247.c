RecordConvertSetToRanges(RecordSetPtr pSet,
                         GetContextRangeInfoPtr pri,
                         int byteoffset,
                         Bool card8, unsigned int imax, int *pStartIndex)
{
    int nRanges;
    RecordSetIteratePtr pIter = NULL;
    RecordSetInterval interval;
    CARD8 *pCARD8;
    CARD16 *pCARD16;
    int err;

    if (!pSet)
        return Success;

    nRanges = pStartIndex ? *pStartIndex : 0;
    while ((pIter = RecordIterateSet(pSet, pIter, &interval))) {
        if (interval.first > imax)
            break;
        if (interval.last > imax)
            interval.last = imax;
        nRanges++;
        if (nRanges > pri->size) {
            err = RecordAllocRanges(pri, nRanges);
            if (err != Success)
                return err;
        }
        else
            pri->nRanges = max(pri->nRanges, nRanges);
        if (card8) {
            pCARD8 = ((CARD8 *) &pri->pRanges[nRanges - 1]) + byteoffset;
            *pCARD8++ = interval.first;
            *pCARD8 = interval.last;
        }
        else {
            pCARD16 = (CARD16 *)
                (((char *) &pri->pRanges[nRanges - 1]) + byteoffset);
            *pCARD16++ = interval.first;
            *pCARD16 = interval.last;
        }
    }
    if (pStartIndex)
        *pStartIndex = nRanges;
    return Success;
}                               /* RecordConvertSetToRanges */