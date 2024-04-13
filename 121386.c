UTF8toLatin1(TScreen *screen, Char *s, unsigned long len, unsigned long *result)
{
    static Char *buffer;
    static Cardinal used;

    Cardinal offset = 0;

    if (len != 0) {
	PtyData data;

	fakePtyData(&data, s, s + len);
	while (decodeUtf8(screen, &data)) {
	    Bool fails = False;
	    Bool extra = False;
	    IChar value;
	    skipPtyData(&data, value);
	    if (value == UCS_REPL) {
		fails = True;
	    } else if (value < 256) {
		AddChar(&buffer, &used, offset, CharOf(value));
	    } else {
		unsigned eqv = ucs2dec(screen, value);
		if (xtermIsDecGraphic(eqv)) {
		    AddChar(&buffer, &used, offset, DECtoASCII(eqv));
		} else {
		    eqv = AsciiEquivs(value);
		    if (eqv == value) {
			fails = True;
		    } else {
			AddChar(&buffer, &used, offset, eqv);
		    }
		    if (isWide((wchar_t) value))
			extra = True;
		}
	    }

	    /*
	     * If we're not able to plug in a single-byte result, insert the
	     * defaultString (which normally is a single "#", but could be
	     * whatever the user wants).
	     */
	    if (fails) {
		const Char *p;

		for (p = (const Char *) screen->default_string; *p != '\0'; ++p) {
		    AddChar(&buffer, &used, offset, *p);
		}
	    }
	    if (extra)
		AddChar(&buffer, &used, offset, ' ');
	}
	AddChar(&buffer, &used, offset, '\0');
	*result = (unsigned long) (offset - 1);
    } else {
	*result = 0;
    }
    return buffer;
}