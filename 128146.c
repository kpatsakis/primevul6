emsg(char *s)
{
    // Skip this if not giving error messages at the moment.
    if (!emsg_not_now())
	return emsg_core((char_u *)s);
    return TRUE;		// no error messages at the moment
}