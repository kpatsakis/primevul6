int compat_printk(const char *fmt, ...)
{
	va_list ap;
	int ret;
	if (!compat_log)
		return 0;
	va_start(ap, fmt);
	ret = vprintk(fmt, ap);
	va_end(ap);
	return ret;
}