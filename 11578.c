NCURSES_PUBLIC_VAR(_nc_tputs_trace) (void)
{
    return CURRENT_SCREEN ? CURRENT_SCREEN->_tputs_trace : _nc_prescreen._tputs_trace;
}