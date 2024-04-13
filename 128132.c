siemsg(const char *s, ...)
{
    if (!emsg_not_now())
    {
	if (IObuff == NULL)
	{
	    // Very early in initialisation and already something wrong, just
	    // give the raw message so the user at least gets a hint.
	    emsg_core((char_u *)s);
	}
	else
	{
	    va_list ap;

	    va_start(ap, s);
	    vim_vsnprintf((char *)IObuff, IOSIZE, s, ap);
	    va_end(ap);
	    emsg_core(IObuff);
	}
    }
# ifdef ABORT_ON_INTERNAL_ERROR
    abort();
# endif
}