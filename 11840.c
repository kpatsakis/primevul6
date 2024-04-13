parse_format(const char *s, char *format, int *len)
{
    *len = 0;
    if (format != 0) {
	bool done = FALSE;
	bool allowminus = FALSE;
	bool dot = FALSE;
	bool err = FALSE;
	char *fmt = format;
	int my_width = 0;
	int my_prec = 0;
	int value = 0;

	*len = 0;
	*format++ = '%';
	while (*s != '\0' && !done) {
	    switch (*s) {
	    case 'c':		/* FALLTHRU */
	    case 'd':		/* FALLTHRU */
	    case 'o':		/* FALLTHRU */
	    case 'x':		/* FALLTHRU */
	    case 'X':		/* FALLTHRU */
	    case 's':
#ifdef EXP_XTERM_1005
	    case 'u':
#endif
		*format++ = *s;
		done = TRUE;
		break;
	    case '.':
		*format++ = *s++;
		if (dot) {
		    err = TRUE;
		} else {	/* value before '.' is the width */
		    dot = TRUE;
		    my_width = value;
		}
		value = 0;
		break;
	    case '#':
		*format++ = *s++;
		break;
	    case ' ':
		*format++ = *s++;
		break;
	    case ':':
		s++;
		allowminus = TRUE;
		break;
	    case '-':
		if (allowminus) {
		    *format++ = *s++;
		} else {
		    done = TRUE;
		}
		break;
	    default:
		if (isdigit(UChar(*s))) {
		    value = (value * 10) + (*s - '0');
		    if (value > 10000)
			err = TRUE;
		    *format++ = *s++;
		} else {
		    done = TRUE;
		}
	    }
	}

	/*
	 * If we found an error, ignore (and remove) the flags.
	 */
	if (err) {
	    my_width = my_prec = value = 0;
	    format = fmt;
	    *format++ = '%';
	    *format++ = *s;
	}

	/*
	 * Any value after '.' is the precision.  If we did not see '.', then
	 * the value is the width.
	 */
	if (dot)
	    my_prec = value;
	else
	    my_width = value;

	*format = '\0';
	/* return maximum string length in print */
	*len = (my_width > my_prec) ? my_width : my_prec;
    }
    return s;
}