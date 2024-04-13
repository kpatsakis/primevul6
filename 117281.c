RecordAllocIntervals(SetInfoPtr psi, int nIntervals)
{
    assert(!psi->intervals);
    psi->intervals = xallocarray(nIntervals, sizeof(RecordSetInterval));
    if (!psi->intervals)
        return BadAlloc;
    memset(psi->intervals, 0, nIntervals * sizeof(RecordSetInterval));
    psi->size = nIntervals;
    return Success;
}                               /* end RecordAllocIntervals */