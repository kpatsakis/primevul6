trace_normalized_cost(NCURSES_SP_DCLx const char *capname, const char *cap, int affcnt)
{
    int result = normalized_cost(NCURSES_SP_ARGx cap, affcnt);
    TR(TRACE_CHARPUT | TRACE_MOVE,
       ("NormalizedCost %s %d %s", capname, result, _nc_visbuf(cap)));
    return result;
}