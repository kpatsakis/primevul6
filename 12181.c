copy_nonerror_line(char_u *linebuf, int linelen, qffields_T *fields)
{
    char_u	*p;

    if (linelen >= fields->errmsglen)
    {
	// linelen + null terminator
	if ((p = vim_realloc(fields->errmsg, linelen + 1)) == NULL)
	    return QF_NOMEM;
	fields->errmsg = p;
	fields->errmsglen = linelen + 1;
    }
    // copy whole line to error message
    vim_strncpy(fields->errmsg, linebuf, linelen);

    return QF_OK;
}