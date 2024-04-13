RecordSwapRanges(xRecordRange * pRanges, int nRanges)
{
    int i;

    for (i = 0; i < nRanges; i++, pRanges++) {
        swaps(&pRanges->extRequestsMinorFirst);
        swaps(&pRanges->extRequestsMinorLast);
        swaps(&pRanges->extRepliesMinorFirst);
        swaps(&pRanges->extRepliesMinorLast);
    }
}                               /* RecordSwapRanges */