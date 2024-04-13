_tracef(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    _nc_va_tracef(fmt, ap);
    va_end(ap);
}