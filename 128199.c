semsg(const char *s, ...)
{
    // Skip this if not giving error messages at the moment.
    if (!emsg_not_now())
    {
	if (IObuff == NULL)
	{
	    // Very early in initialisation and already something wrong, just
	    // give the raw message so the user at least gets a hint.
	    return emsg_core((char_u *)s);
	}
	else
	{
	    va_list ap;

	    va_start(ap, s);
	    vim_vsnprintf((char *)IObuff, IOSIZE, s, ap);
	    va_end(ap);
	    return emsg_core(IObuff);
	}
    }
    return TRUE;		// no error messages at the moment
}