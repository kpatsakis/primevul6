check_acs(TERMTYPE2 *tp)
{
    int vt100_smacs = 0;
    int vt100_rmacs = 0;
    int vt100_enacs = 0;

    /*
     * ena_acs is not always necessary, but if it is present, the enter/exit
     * capabilities should be.
     */
    ANDMISSING(ena_acs, enter_alt_charset_mode);
    ANDMISSING(ena_acs, exit_alt_charset_mode);
    PAIRED(exit_alt_charset_mode, exit_alt_charset_mode);

    /*
     * vt100-like is frequently used, but perhaps ena_acs is missing, etc.
     */
    if (VALID_STRING(enter_alt_charset_mode)) {
	vt100_smacs = (!strcmp("\033(0", enter_alt_charset_mode)
		       ? 2
		       : (!strcmp("\016", enter_alt_charset_mode)
			  ? 1
			  : 0));
    }
    if (VALID_STRING(exit_alt_charset_mode)) {
	vt100_rmacs = (!strcmp("\033(B", exit_alt_charset_mode)
		       ? 2
		       : (!strcmp("\017", exit_alt_charset_mode)
			  ? 1
			  : 0));
    }
    if (VALID_STRING(ena_acs)) {
	vt100_enacs = (!strcmp("\033(B\033)0", ena_acs)
		       ? 2
		       : 0);
    }
    if (vt100_rmacs && vt100_smacs && (vt100_rmacs != vt100_smacs)) {
	_nc_warning("rmacs/smacs are inconsistent");
    }
    if ((vt100_rmacs == 2) && (vt100_smacs == 2) && vt100_enacs) {
	_nc_warning("rmacs/smacs make enacs redundant");
    }
    if ((vt100_rmacs == 1) && (vt100_smacs == 1) && !vt100_enacs) {
	_nc_warning("VT100-style rmacs/smacs require enacs");
    }

    if (VALID_STRING(acs_chars)) {
	const char *boxes = "lmkjtuvwqxn";
	char mapped[256];
	char missing[256];
	const char *p;
	char *q;

	memset(mapped, 0, sizeof(mapped));
	for (p = acs_chars; *p != '\0'; p += 2) {
	    if (p[1] == '\0') {
		_nc_warning("acsc has odd number of characters");
		break;
	    }
	    mapped[UChar(p[0])] = p[1];
	}

	if (mapped[UChar('I')] && !mapped[UChar('i')]) {
	    _nc_warning("acsc refers to 'I', which is probably an error");
	}

	for (p = boxes, q = missing; *p != '\0'; ++p) {
	    if (!mapped[UChar(p[0])]) {
		*q++ = p[0];
	    }
	}
	*q = '\0';

	assert(strlen(missing) <= strlen(boxes));
	if (*missing != '\0' && strcmp(missing, boxes)) {
	    _nc_warning("acsc is missing some line-drawing mapping: %s", missing);
	}
    }
}