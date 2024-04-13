_nc_locked_tracef(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _nc_va_tracef(fmt, ap);
    va_end(ap);

    if (--(MyNested) == 0) {
	_nc_unlock_global(tracef);
    }
}