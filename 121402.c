alloc8bitTargets(Widget w, TScreen *screen)
{
    Atom **resultp = &(screen->selection_targets_8bit);

    if (*resultp == 0) {
	Atom *result = 0;

	if (!overrideTargets(w, screen->eightbit_select_types, &result)) {
	    result = TypeXtMallocN(Atom, 5);
	    if (result == NULL) {
		TRACE(("Couldn't allocate 8bit selection targets\n"));
	    } else {
		int n = 0;

		if (XSupportsLocale()) {
#ifdef X_HAVE_UTF8_STRING
		    result[n++] = XA_UTF8_STRING(XtDisplay(w));
#endif
		    if (screen->i18nSelections) {
			result[n++] = XA_TEXT(XtDisplay(w));
			result[n++] = XA_COMPOUND_TEXT(XtDisplay(w));
		    }
		}
		result[n++] = XA_STRING;
		result[n] = None;
	    }
	}

	*resultp = result;
    }

    return *resultp;
}