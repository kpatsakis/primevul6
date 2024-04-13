efm_to_regpat(
	char_u	*efm,
	int	len,
	efm_T	*fmt_ptr,
	char_u	*regpat)
{
    char_u	*ptr;
    char_u	*efmp;
    int		round;
    int		idx = 0;

    // Build a regexp pattern for a 'errorformat' option part
    ptr = regpat;
    *ptr++ = '^';
    round = 0;
    for (efmp = efm; efmp < efm + len; ++efmp)
    {
	if (*efmp == '%')
	{
	    ++efmp;
	    for (idx = 0; idx < FMT_PATTERNS; ++idx)
		if (fmt_pat[idx].convchar == *efmp)
		    break;
	    if (idx < FMT_PATTERNS)
	    {
		ptr = efmpat_to_regpat(efmp, ptr, fmt_ptr, idx, round);
		if (ptr == NULL)
		    return FAIL;
		round++;
	    }
	    else if (*efmp == '*')
	    {
		++efmp;
		ptr = scanf_fmt_to_regpat(&efmp, efm, len, ptr);
		if (ptr == NULL)
		    return FAIL;
	    }
	    else if (vim_strchr((char_u *)"%\\.^$~[", *efmp) != NULL)
		*ptr++ = *efmp;		// regexp magic characters
	    else if (*efmp == '#')
		*ptr++ = '*';
	    else if (*efmp == '>')
		fmt_ptr->conthere = TRUE;
	    else if (efmp == efm + 1)		// analyse prefix
	    {
		// prefix is allowed only at the beginning of the errorformat
		// option part
		efmp = efm_analyze_prefix(efmp, fmt_ptr);
		if (efmp == NULL)
		    return FAIL;
	    }
	    else
	    {
		semsg(_(e_invalid_chr_in_format_string), *efmp);
		return FAIL;
	    }
	}
	else			// copy normal character
	{
	    if (*efmp == '\\' && efmp + 1 < efm + len)
		++efmp;
	    else if (vim_strchr((char_u *)".*^$~[", *efmp) != NULL)
		*ptr++ = '\\';	// escape regexp atoms
	    if (*efmp)
		*ptr++ = *efmp;
	}
    }
    *ptr++ = '$';
    *ptr = NUL;

    return OK;
}