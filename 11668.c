NCURSES_SP_NAME(_nc_msec_cost) (NCURSES_SP_DCLx const char *const cap, int affcnt)
/* compute the cost of a given operation */
{
    if (cap == 0)
	return (INFINITY);
    else {
	const char *cp;
	float cum_cost = 0.0;

	for (cp = cap; *cp; cp++) {
	    /* extract padding, either mandatory or required */
	    if (cp[0] == '$' && cp[1] == '<' && strchr(cp, '>')) {
		float number = 0.0;

		for (cp += 2; *cp != '>'; cp++) {
		    if (isdigit(UChar(*cp)))
			number = number * 10 + (float) (*cp - '0');
		    else if (*cp == '*')
			number *= (float) affcnt;
		    else if (*cp == '.' && (*++cp != '>') && isdigit(UChar(*cp)))
			number += (float) ((*cp - '0') / 10.0);
		}

#if NCURSES_NO_PADDING
		if (!GetNoPadding(SP_PARM))
#endif
		    cum_cost += number * 10;
	    } else if (SP_PARM) {
		cum_cost += (float) SP_PARM->_char_padding;
	    }
	}

	return ((int) cum_cost);
    }
}