RecordConvertMinorOpInfoToRanges(RecordMinorOpPtr pMinOpInfo,
                                 GetContextRangeInfoPtr pri, int byteoffset)
{
    int nsets;
    int start;
    int i;
    int err;

    if (!pMinOpInfo)
        return Success;

    nsets = pMinOpInfo->count;
    pMinOpInfo++;
    start = 0;
    for (i = 0; i < nsets; i++) {
        int j, s;

        s = start;
        err = RecordConvertSetToRanges(pMinOpInfo[i].major.pMinOpSet, pri,
                                       byteoffset + 2, FALSE, 65535, &start);
        if (err != Success)
            return err;
        for (j = s; j < start; j++) {
            CARD8 *pCARD8 = ((CARD8 *) &pri->pRanges[j]) + byteoffset;

            *pCARD8++ = pMinOpInfo[i].major.first;
            *pCARD8 = pMinOpInfo[i].major.last;
        }
    }
    return Success;
}                               /* RecordConvertMinorOpInfoToRanges */