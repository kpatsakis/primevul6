trace_cost_of(NCURSES_SP_DCLx const char *capname, const char *cap, int affcnt)
{
    int result = NCURSES_SP_NAME(_nc_msec_cost) (NCURSES_SP_ARGx cap, affcnt);
    TR(TRACE_CHARPUT | TRACE_MOVE,
       ("CostOf %s %d %s", capname, result, _nc_visbuf(cap)));
    return result;
}