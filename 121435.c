ConvertSelection(Widget w,
		 Atom *selection,
		 Atom *target,
		 Atom *type,
		 XtPointer *value,
		 unsigned long *length,
		 int *format)
{
    Display *dpy = XtDisplay(w);
    TScreen *screen;
    SelectedCells *scp;
    Bool result = False;

    Char *data;
    unsigned long data_length;

    XtermWidget xw;

    if ((xw = getXtermWidget(w)) == 0)
	return False;

    screen = TScreenOf(xw);

    TRACE(("ConvertSelection %s -> %s\n",
	   TraceAtomName(screen->display, *selection),
	   visibleSelectionTarget(dpy, *target)));

    if (keepClipboard(dpy, *selection)) {
	TRACE(("asked for clipboard\n"));
	scp = &(screen->clipboard_data);
    } else {
	TRACE(("asked for selection\n"));
	scp = &(screen->selected_cells[AtomToSelection(dpy, *selection)]);
    }

    data = scp->data_buffer;
    data_length = scp->data_length;
    if (data == NULL) {
	TRACE(("...no selection-data\n"));
	return False;
    }

    if (*target == XA_TARGETS(dpy)) {
	Atom *targetP;
	XPointer std_return = 0;
	unsigned long std_length;

	if (XmuConvertStandardSelection(w, screen->selection_time, selection,
					target, type, &std_return,
					&std_length, format)) {
	    Atom *my_targets = _SelectionTargets(w);
	    Atom *allocP;
	    Atom *std_targets;

	    TRACE(("XmuConvertStandardSelection - success\n"));
	    std_targets = (Atom *) (void *) (std_return);
	    *length = std_length + 6;

	    targetP = TypeXtMallocN(Atom, *length);
	    allocP = targetP;

	    *value = (XtPointer) targetP;

	    if (my_targets != 0) {
		while (*my_targets != None) {
		    *targetP++ = *my_targets++;
		}
	    }
	    *targetP++ = XA_LENGTH(dpy);
	    *targetP++ = XA_LIST_LENGTH(dpy);

	    *length = std_length + (unsigned long) (targetP - allocP);

	    memcpy(targetP, std_targets, sizeof(Atom) * std_length);
	    XtFree((char *) std_targets);
	    *type = XA_ATOM;
	    *format = 32;
	    result = True;
	} else {
	    TRACE(("XmuConvertStandardSelection - failed\n"));
	}
    }
#if OPT_WIDE_CHARS
    else if (screen->wide_chars && *target == XA_STRING) {
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    Xutf8TextListToTextProperty,
				    XStringStyle);
	TRACE(("...Xutf8TextListToTextProperty:%d\n", result));
    } else if (screen->wide_chars && *target == XA_UTF8_STRING(dpy)) {
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    Xutf8TextListToTextProperty,
				    XUTF8StringStyle);
	TRACE(("...Xutf8TextListToTextProperty:%d\n", result));
    } else if (screen->wide_chars && *target == XA_TEXT(dpy)) {
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    Xutf8TextListToTextProperty,
				    XStdICCTextStyle);
	TRACE(("...Xutf8TextListToTextProperty:%d\n", result));
    } else if (screen->wide_chars && *target == XA_COMPOUND_TEXT(dpy)) {
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    Xutf8TextListToTextProperty,
				    XCompoundTextStyle);
	TRACE(("...Xutf8TextListToTextProperty:%d\n", result));
    }
#endif

    else if (*target == XA_STRING) {	/* not wide_chars */
	/* We can only reach this point if the selection requestor
	   requested STRING before any of TEXT, COMPOUND_TEXT or
	   UTF8_STRING.  We therefore assume that the requestor is not
	   properly internationalised, and dump raw eight-bit data
	   with no conversion into the selection.  Yes, this breaks
	   the ICCCM in non-Latin-1 locales. */
	*type = XA_STRING;
	*value = (XtPointer) data;
	*length = data_length;
	*format = 8;
	result = True;
	TRACE(("...raw 8-bit data:%d\n", result));
    } else if (*target == XA_TEXT(dpy)) {	/* not wide_chars */
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    XmbTextListToTextProperty,
				    XStdICCTextStyle);
	TRACE(("...XmbTextListToTextProperty(StdICC):%d\n", result));
    } else if (*target == XA_COMPOUND_TEXT(dpy)) {	/* not wide_chars */
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    XmbTextListToTextProperty,
				    XCompoundTextStyle);
	TRACE(("...XmbTextListToTextProperty(Compound):%d\n", result));
    }
#ifdef X_HAVE_UTF8_STRING
    else if (*target == XA_UTF8_STRING(dpy)) {	/* not wide_chars */
	result =
	    _ConvertSelectionHelper(w, scp,
				    type, value, length, format,
				    XmbTextListToTextProperty,
				    XUTF8StringStyle);
	TRACE(("...XmbTextListToTextProperty(UTF8):%d\n", result));
    }
#endif
    else if (*target == XA_LIST_LENGTH(dpy)) {
	result = SaveConvertedLength(value, (unsigned long) 1);
	*type = XA_INTEGER;
	*length = 1;
	*format = 32;
	TRACE(("...list of values:%d\n", result));
    } else if (*target == XA_LENGTH(dpy)) {
	/* This value is wrong if we have UTF-8 text */
	result = SaveConvertedLength(value, scp->data_length);
	*type = XA_INTEGER;
	*length = 1;
	*format = 32;
	TRACE(("...list of values:%d\n", result));
    } else if (XmuConvertStandardSelection(w,
					   screen->selection_time, selection,
					   target, type, (XPointer *) value,
					   length, format)) {
	result = True;
	TRACE(("...XmuConvertStandardSelection:%d\n", result));
    }

    /* else */
    return (Boolean) result;
}