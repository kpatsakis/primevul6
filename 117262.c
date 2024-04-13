RecordConvertRangesToIntervals(SetInfoPtr psi,
                               xRecordRange * pRanges,
                               int nRanges,
                               int byteoffset,
                               SetInfoPtr pExtSetInfo, int *pnExtSetInfo)
{
    int i;
    CARD8 *pCARD8;
    int first, last;
    int err;

    for (i = 0; i < nRanges; i++, pRanges++) {
        pCARD8 = ((CARD8 *) pRanges) + byteoffset;
        first = pCARD8[0];
        last = pCARD8[1];
        if (first || last) {
            if (!psi->intervals) {
                err = RecordAllocIntervals(psi, 2 * (nRanges - i));
                if (err != Success)
                    return err;
            }
            psi->intervals[psi->nintervals].first = first;
            psi->intervals[psi->nintervals].last = last;
            psi->nintervals++;
            assert(psi->nintervals <= psi->size);
            if (pExtSetInfo) {
                SetInfoPtr pesi = pExtSetInfo;
                CARD16 *pCARD16 = (CARD16 *) (pCARD8 + 2);
                int j;

                for (j = 0; j < *pnExtSetInfo; j++, pesi++) {
                    if ((first == pesi->first) && (last == pesi->last))
                        break;
                }
                if (j == *pnExtSetInfo) {
                    err = RecordAllocIntervals(pesi, 2 * (nRanges - i));
                    if (err != Success)
                        return err;
                    pesi->first = first;
                    pesi->last = last;
                    (*pnExtSetInfo)++;
                }
                pesi->intervals[pesi->nintervals].first = pCARD16[0];
                pesi->intervals[pesi->nintervals].last = pCARD16[1];
                pesi->nintervals++;
                assert(pesi->nintervals <= pesi->size);
            }
        }
    }
    return Success;
}                               /* end RecordConvertRangesToIntervals */