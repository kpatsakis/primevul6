allocUtf8Targets(Widget w, TScreen *screen)
{
    Atom **resultp = &(screen->selection_targets_utf8);

    if (*resultp == 0) {
	Atom *result;

	if (!overrideTargets(w, screen->utf8_select_types, &result)) {
	    result = TypeXtMallocN(Atom, 5);
	    if (result == NULL) {
		TRACE(("Couldn't allocate utf-8 selection targets\n"));
	    } else {
		int n = 0;

		if (XSupportsLocale()) {
		    result[n++] = XA_UTF8_STRING(XtDisplay(w));
#ifdef X_HAVE_UTF8_STRING
		    if (screen->i18nSelections) {
			result[n++] = XA_TEXT(XtDisplay(w));
			result[n++] = XA_COMPOUND_TEXT(XtDisplay(w));
		    }
#endif
		}
		result[n++] = XA_STRING;
		result[n] = None;
	    }
	}

	*resultp = result;
    }

    return *resultp;
}