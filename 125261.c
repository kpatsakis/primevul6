PHP_RSHUTDOWN_FUNCTION(gd)
{
	gdFontCacheShutdown();
	return SUCCESS;
}