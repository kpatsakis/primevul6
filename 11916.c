_nc_count_outchars(long increment)
{
    if (CURRENT_SCREEN)
	CURRENT_SCREEN->_outchars += increment;
    else
	_nc_prescreen._outchars += increment;
}