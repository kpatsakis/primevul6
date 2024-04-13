init_direct_colors(NCURSES_SP_DCL0)
{
    static NCURSES_CONST char name[] = "RGB";

    int n;
    const char *s;
    int width;
    rgb_bits_t *result = &(SP_PARM->_direct_color);

    result->value = 0;

    if (COLORS >= 8) {
	/* find the number of bits needed for the maximum color value */
	for (width = 0; (1 << width) - 1 < (COLORS - 1); ++width) {
	    ;
	}

	if ((n = tigetflag(name)) > 0) {
	    n = (width + 2) / 3;
	    result->bits.red = UChar(n);
	    result->bits.green = UChar(n);
	    result->bits.blue = UChar(width - (2 * n));
	} else if ((n = tigetnum(name)) > 0) {
	    result->bits.red = UChar(n);
	    result->bits.green = UChar(n);
	    result->bits.blue = UChar(n);
	} else if ((s = tigetstr(name)) != 0 && VALID_STRING(s)) {
	    int red = n;
	    int green = n;
	    int blue = width - (2 * n);

	    switch (sscanf(s, "%d/%d/%d", &red, &green, &blue)) {
	    default:
		blue = width - (2 * n);
		/* FALLTHRU */
	    case 1:
		green = n;
		/* FALLTHRU */
	    case 2:
		red = n;
		/* FALLTHRU */
	    case 3:
		/* okay */
		break;
	    }
	    result->bits.red = UChar(red);
	    result->bits.green = UChar(green);
	    result->bits.blue = UChar(blue);
	}
    }
    return (result->value != 0);
}