TransformLine(NCURSES_SP_DCLx int const lineno)
{
    int firstChar, oLastChar, nLastChar;
    NCURSES_CH_T *newLine = NewScreen(SP_PARM)->_line[lineno].text;
    NCURSES_CH_T *oldLine = CurScreen(SP_PARM)->_line[lineno].text;
    int n;
    bool attrchanged = FALSE;

    TR(TRACE_UPDATE, (T_CALLED("TransformLine(%p, %d)"), (void *) SP_PARM, lineno));

    /* copy new hash value to old one */
    if (SP_PARM->oldhash && SP_PARM->newhash)
	SP_PARM->oldhash[lineno] = SP_PARM->newhash[lineno];

    /*
     * If we have colors, there is the possibility of having two color pairs
     * that display as the same colors.  For instance, Lynx does this.  Check
     * for this case, and update the old line with the new line's colors when
     * they are equivalent.
     */
    if (SP_PARM->_coloron) {
	int oldPair;
	int newPair;

	for (n = 0; n < screen_columns(SP_PARM); n++) {
	    if (!CharEq(newLine[n], oldLine[n])) {
		oldPair = GetPair(oldLine[n]);
		newPair = GetPair(newLine[n]);
		if (oldPair != newPair
		    && unColor(oldLine[n]) == unColor(newLine[n])) {
		    if (oldPair < SP_PARM->_pair_limit
			&& newPair < SP_PARM->_pair_limit
			&& (isSamePair(SP_PARM->_color_pairs[oldPair],
				       SP_PARM->_color_pairs[newPair]))) {
			SetPair(oldLine[n], GetPair(newLine[n]));
		    }
		}
	    }
	}
    }

    if (ceol_standout_glitch && clr_eol) {
	firstChar = 0;
	while (firstChar < screen_columns(SP_PARM)) {
	    if (!SameAttrOf(newLine[firstChar], oldLine[firstChar])) {
		attrchanged = TRUE;
		break;
	    }
	    firstChar++;
	}
    }

    firstChar = 0;

    if (attrchanged) {		/* we may have to disregard the whole line */
	GoTo(NCURSES_SP_ARGx lineno, firstChar);
	ClrToEOL(NCURSES_SP_ARGx
		 ClrBlank(NCURSES_SP_ARGx
			  CurScreen(SP_PARM)), FALSE);
	PutRange(NCURSES_SP_ARGx
		 oldLine, newLine, lineno, 0,
		 screen_columns(SP_PARM) - 1);
#if USE_XMC_SUPPORT

	/*
	 * This is a very simple loop to paint characters which may have the
	 * magic cookie glitch embedded.  It doesn't know much about video
	 * attributes which are continued from one line to the next.  It
	 * assumes that we have filtered out requests for attribute changes
	 * that do not get mapped to blank positions.
	 *
	 * FIXME: we are not keeping track of where we put the cookies, so this
	 * will work properly only once, since we may overwrite a cookie in a
	 * following operation.
	 */
    } else if (magic_cookie_glitch > 0) {
	GoTo(NCURSES_SP_ARGx lineno, firstChar);
	for (n = 0; n < screen_columns(SP_PARM); n++) {
	    int m = n + magic_cookie_glitch;

	    /* check for turn-on:
	     * If we are writing an attributed blank, where the
	     * previous cell is not attributed.
	     */
	    if (ISBLANK(newLine[n])
		&& ((n > 0
		     && xmc_turn_on(SP_PARM, newLine[n - 1], newLine[n]))
		    || (n == 0
			&& lineno > 0
			&& xmc_turn_on(SP_PARM,
				       xmc_new(SP_PARM, lineno - 1,
					       screen_columns(SP_PARM) - 1),
				       newLine[n])))) {
		n = m;
	    }

	    PutChar(NCURSES_SP_ARGx CHREF(newLine[n]));

	    /* check for turn-off:
	     * If we are writing an attributed non-blank, where the
	     * next cell is blank, and not attributed.
	     */
	    if (!ISBLANK(newLine[n])
		&& ((n + 1 < screen_columns(SP_PARM)
		     && xmc_turn_off(SP_PARM, newLine[n], newLine[n + 1]))
		    || (n + 1 >= screen_columns(SP_PARM)
			&& lineno + 1 < screen_lines(SP_PARM)
			&& xmc_turn_off(SP_PARM,
					newLine[n],
					xmc_new(SP_PARM, lineno + 1, 0))))) {
		n = m;
	    }

	}
#endif
    } else {
	NCURSES_CH_T blank;

	/* it may be cheap to clear leading whitespace with clr_bol */
	blank = newLine[0];
	if (clr_bol && can_clear_with(NCURSES_SP_ARGx CHREF(blank))) {
	    int oFirstChar, nFirstChar;

	    for (oFirstChar = 0;
		 oFirstChar < screen_columns(SP_PARM);
		 oFirstChar++)
		if (!CharEq(oldLine[oFirstChar], blank))
		    break;
	    for (nFirstChar = 0;
		 nFirstChar < screen_columns(SP_PARM);
		 nFirstChar++)
		if (!CharEq(newLine[nFirstChar], blank))
		    break;

	    if (nFirstChar == oFirstChar) {
		firstChar = nFirstChar;
		/* find the first differing character */
		while (firstChar < screen_columns(SP_PARM)
		       && CharEq(newLine[firstChar], oldLine[firstChar]))
		    firstChar++;
	    } else if (oFirstChar > nFirstChar) {
		firstChar = nFirstChar;
	    } else {		/* oFirstChar < nFirstChar */
		firstChar = oFirstChar;
		if (SP_PARM->_el1_cost < nFirstChar - oFirstChar) {
		    if (nFirstChar >= screen_columns(SP_PARM)
			&& SP_PARM->_el_cost <= SP_PARM->_el1_cost) {
			GoTo(NCURSES_SP_ARGx lineno, 0);
			UpdateAttrs(SP_PARM, blank);
			NCURSES_PUTP2("clr_eol", clr_eol);
		    } else {
			GoTo(NCURSES_SP_ARGx lineno, nFirstChar - 1);
			UpdateAttrs(SP_PARM, blank);
			NCURSES_PUTP2("clr_bol", clr_bol);
		    }

		    while (firstChar < nFirstChar)
			oldLine[firstChar++] = blank;
		}
	    }
	} else {
	    /* find the first differing character */
	    while (firstChar < screen_columns(SP_PARM)
		   && CharEq(newLine[firstChar], oldLine[firstChar]))
		firstChar++;
	}
	/* if there wasn't one, we're done */
	if (firstChar >= screen_columns(SP_PARM)) {
	    TR(TRACE_UPDATE, (T_RETURN("")));
	    return;
	}

	blank = newLine[screen_columns(SP_PARM) - 1];

	if (!can_clear_with(NCURSES_SP_ARGx CHREF(blank))) {
	    /* find the last differing character */
	    nLastChar = screen_columns(SP_PARM) - 1;

	    while (nLastChar > firstChar
		   && CharEq(newLine[nLastChar], oldLine[nLastChar]))
		nLastChar--;

	    if (nLastChar >= firstChar) {
		GoTo(NCURSES_SP_ARGx lineno, firstChar);
		PutRange(NCURSES_SP_ARGx
			 oldLine,
			 newLine,
			 lineno,
			 firstChar,
			 nLastChar);
		memcpy(oldLine + firstChar,
		       newLine + firstChar,
		       (unsigned) (nLastChar - firstChar + 1) * sizeof(NCURSES_CH_T));
	    }
	    TR(TRACE_UPDATE, (T_RETURN("")));
	    return;
	}

	/* find last non-blank character on old line */
	oLastChar = screen_columns(SP_PARM) - 1;
	while (oLastChar > firstChar && CharEq(oldLine[oLastChar], blank))
	    oLastChar--;

	/* find last non-blank character on new line */
	nLastChar = screen_columns(SP_PARM) - 1;
	while (nLastChar > firstChar && CharEq(newLine[nLastChar], blank))
	    nLastChar--;

	if ((nLastChar == firstChar)
	    && (SP_PARM->_el_cost < (oLastChar - nLastChar))) {
	    GoTo(NCURSES_SP_ARGx lineno, firstChar);
	    if (!CharEq(newLine[firstChar], blank))
		PutChar(NCURSES_SP_ARGx CHREF(newLine[firstChar]));
	    ClrToEOL(NCURSES_SP_ARGx blank, FALSE);
	} else if ((nLastChar != oLastChar)
		   && (!CharEq(newLine[nLastChar], oldLine[oLastChar])
		       || !(SP_PARM->_nc_sp_idcok
			    && NCURSES_SP_NAME(has_ic) (NCURSES_SP_ARG)))) {
	    GoTo(NCURSES_SP_ARGx lineno, firstChar);
	    if ((oLastChar - nLastChar) > SP_PARM->_el_cost) {
		if (PutRange(NCURSES_SP_ARGx
			     oldLine,
			     newLine,
			     lineno,
			     firstChar,
			     nLastChar)) {
		    GoTo(NCURSES_SP_ARGx lineno, nLastChar + 1);
		}
		ClrToEOL(NCURSES_SP_ARGx blank, FALSE);
	    } else {
		n = max(nLastChar, oLastChar);
		PutRange(NCURSES_SP_ARGx
			 oldLine,
			 newLine,
			 lineno,
			 firstChar,
			 n);
	    }
	} else {
	    int nLastNonblank = nLastChar;
	    int oLastNonblank = oLastChar;

	    /* find the last characters that really differ */
	    /* can be -1 if no characters differ */
	    while (CharEq(newLine[nLastChar], oldLine[oLastChar])) {
		/* don't split a wide char */
		if (isWidecExt(newLine[nLastChar]) &&
		    !CharEq(newLine[nLastChar - 1], oldLine[oLastChar - 1]))
		    break;
		nLastChar--;
		oLastChar--;
		if (nLastChar == -1 || oLastChar == -1)
		    break;
	    }

	    n = min(oLastChar, nLastChar);
	    if (n >= firstChar) {
		GoTo(NCURSES_SP_ARGx lineno, firstChar);
		PutRange(NCURSES_SP_ARGx
			 oldLine,
			 newLine,
			 lineno,
			 firstChar,
			 n);
	    }

	    if (oLastChar < nLastChar) {
		int m = max(nLastNonblank, oLastNonblank);
#if USE_WIDEC_SUPPORT
		if (n) {
		    while (isWidecExt(newLine[n + 1]) && n) {
			--n;
			--oLastChar;	/* increase cost */
		    }
		} else if (n >= firstChar &&
			   isWidecBase(newLine[n])) {
		    while (isWidecExt(newLine[n + 1])) {
			++n;
			++oLastChar;	/* decrease cost */
		    }
		}
#endif
		GoTo(NCURSES_SP_ARGx lineno, n + 1);
		if ((nLastChar < nLastNonblank)
		    || InsCharCost(SP_PARM, nLastChar - oLastChar) > (m - n)) {
		    PutRange(NCURSES_SP_ARGx
			     oldLine,
			     newLine,
			     lineno,
			     n + 1,
			     m);
		} else {
		    InsStr(NCURSES_SP_ARGx &newLine[n + 1], nLastChar - oLastChar);
		}
	    } else if (oLastChar > nLastChar) {
		GoTo(NCURSES_SP_ARGx lineno, n + 1);
		if (DelCharCost(SP_PARM, oLastChar - nLastChar)
		    > SP_PARM->_el_cost + nLastNonblank - (n + 1)) {
		    if (PutRange(NCURSES_SP_ARGx oldLine, newLine, lineno,
				 n + 1, nLastNonblank)) {
			GoTo(NCURSES_SP_ARGx lineno, nLastNonblank + 1);
		    }
		    ClrToEOL(NCURSES_SP_ARGx blank, FALSE);
		} else {
		    /*
		     * The delete-char sequence will
		     * effectively shift in blanks from the
		     * right margin of the screen.  Ensure
		     * that they are the right color by
		     * setting the video attributes from
		     * the last character on the row.
		     */
		    UpdateAttrs(SP_PARM, blank);
		    DelChar(NCURSES_SP_ARGx oLastChar - nLastChar);
		}
	    }
	}
    }

    /* update the code's internal representation */
    if (screen_columns(SP_PARM) > firstChar)
	memcpy(oldLine + firstChar,
	       newLine + firstChar,
	       (unsigned) (screen_columns(SP_PARM) - firstChar) * sizeof(NCURSES_CH_T));
    TR(TRACE_UPDATE, (T_RETURN("")));
    return;
}