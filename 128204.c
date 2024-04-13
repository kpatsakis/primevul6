smsg(const char *s, ...)
{
    if (IObuff == NULL)
    {
	// Very early in initialisation and already something wrong, just
	// give the raw message so the user at least gets a hint.
	return msg((char *)s);
    }
    else
    {
	va_list arglist;

	va_start(arglist, s);
	vim_vsnprintf((char *)IObuff, IOSIZE, s, arglist);
	va_end(arglist);
	return msg((char *)IObuff);
    }
}