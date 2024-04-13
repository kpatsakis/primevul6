badword_captype(char_u *word, char_u *end)
{
    int		flags = captype(word, end);
    int		c;
    int		l, u;
    int		first;
    char_u	*p;

    if (flags & WF_KEEPCAP)
    {
	// Count the number of UPPER and lower case letters.
	l = u = 0;
	first = FALSE;
	for (p = word; p < end; MB_PTR_ADV(p))
	{
	    c = PTR2CHAR(p);
	    if (SPELL_ISUPPER(c))
	    {
		++u;
		if (p == word)
		    first = TRUE;
	    }
	    else
		++l;
	}

	// If there are more UPPER than lower case letters suggest an
	// ALLCAP word.  Otherwise, if the first letter is UPPER then
	// suggest ONECAP.  Exception: "ALl" most likely should be "All",
	// require three upper case letters.
	if (u > l && u > 2)
	    flags |= WF_ALLCAP;
	else if (first)
	    flags |= WF_ONECAP;

	if (u >= 2 && l >= 2)	// maCARONI maCAroni
	    flags |= WF_MIXCAP;
    }
    return flags;
}