_nc_set_tputs_trace(const char *s)
{
    if (CURRENT_SCREEN)
	CURRENT_SCREEN->_tputs_trace = s;
    else
	_nc_prescreen._tputs_trace = s;
}