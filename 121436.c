xtermButtonInit(XtermWidget xw)
{
    Widget w = (Widget) xw;
    XErrorHandler save = XSetErrorHandler(ignore_x11_error);
    XtTranslations xlations;
    Widget xcelerat;
    String result;

    XtVaGetValues(w,
		  XtNtranslations, &xlations,
		  XtNaccelerators, &xcelerat,
		  (XtPointer) 0);
    result = _XtPrintXlations(w, xlations, xcelerat, True);
    if (result) {
	static const char *table[] =
	{
	    "insert-selection",
	    "select-end",
	    "select-extend",
	    "select-start",
	    "start-extend",
	};
	char *data = x_strdup(result);
	char *next;
	int state = scanMods;
	int state2 = scanMods;
	unsigned first;
	unsigned last;
	int have_button = -1;
	Bool want_button = False;
	Bool have_shift = False;
	unsigned allowed = 0;
	unsigned disallow = 0;

	TRACE(("xtermButtonInit length %ld\n", strlen(result)));
	xw->keyboard.print_translations = data;
	while ((next = scanTrans(data, &state, &state2, &first, &last)) != 0) {
	    unsigned len = (last - first);
	    TRACE2(("parse %s:%d..%d '%.*s'\n",
		    visibleScan(state), first, last,
		    len, data + first));
	    if (state == scanMods) {
		if (len > 1 && data[first] == '~') {
		    len--;
		    first++;
		}
		if (len == 7 && !x_strncasecmp(data + first, "button", len - 1)) {
		    have_button = data[first + 6] - '0';
		} else if (len == 5 && !x_strncasecmp(data + first, "shift", len)) {
		    have_shift = True;
		}
	    } else if (state == scanKey) {
		if (!x_strncasecmp(data + first, "<buttonpress>", len) ||
		    !x_strncasecmp(data + first, "<buttonrelease>", len)) {
		    want_button = True;
		} else if (want_button) {
		    have_button = data[first] - '0';
		    want_button = False;
		}
	    } else if (state == scanFunc && have_button > 0) {
		Cardinal n;
		unsigned bmask = 1U << (have_button - 1);
		for (n = 0; n < XtNumber(table); ++n) {
		    if (!x_strncasecmp(table[n], data + first, len)) {
			TRACE(("...button %d: %s%s\n",
			       have_button, table[n],
			       have_shift ? " (disallow)" : ""));
			if (have_shift)
			    disallow |= bmask;
			else
			    allowed |= bmask;
			break;
		    }
		}
	    }
	    if (state2 == scanMods && state >= scanColon) {
		have_button = -1;
		want_button = False;
		have_shift = False;
	    }
	    state = state2;
	    data = next;
	}
	XFree((char *) result);
	xw->keyboard.shift_buttons = allowed & ~disallow;
#if OPT_TRACE
	if (xw->keyboard.shift_buttons) {
	    int button = 0;
	    unsigned mask = xw->keyboard.shift_buttons;
	    TRACE(("...Buttons used for selection that can be overridden:"));
	    while (mask != 0) {
		++button;
		if ((mask & 1) != 0)
		    TRACE((" %d", button));
		mask >>= 1;
	    }
	    TRACE(("\n"));
	} else {
	    TRACE(("...No buttons used with selection can be overridden\n"));
	}
#endif
    }
    XSetErrorHandler(save);
}