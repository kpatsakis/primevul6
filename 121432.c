overrideTargets(Widget w, String value, Atom **resultp)
{
    Bool override = False;
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) != 0) {
	TScreen *screen = TScreenOf(xw);

	if (!IsEmpty(value)) {
	    char *copied = x_strdup(value);
	    if (copied != 0) {
		Atom *result = 0;
		Cardinal count = 1;
		int n;

		TRACE(("decoding SelectTypes \"%s\"\n", value));
		for (n = 0; copied[n] != '\0'; ++n) {
		    if (copied[n] == ',')
			++count;
		}
		result = TypeXtMallocN(Atom, (2 * count) + 1);
		if (result == NULL) {
		    TRACE(("Couldn't allocate selection types\n"));
		} else {
		    char nextc = '?';
		    char *listp = (char *) copied;
		    count = 0;
		    do {
			char *nextp = parseItem(listp, &nextc);
			char *item = x_strtrim(listp);
			size_t len = (item ? strlen(item) : 0);

			if (len == 0) {
			    /* EMPTY */ ;
			}
#if OPT_WIDE_CHARS
			else if (sameItem(item, "UTF8")) {
			    result[count++] = XA_UTF8_STRING(XtDisplay(w));
			}
#endif
			else if (sameItem(item, "I18N")) {
			    if (screen->i18nSelections) {
				result[count++] = XA_TEXT(XtDisplay(w));
				result[count++] = XA_COMPOUND_TEXT(XtDisplay(w));
			    }
			} else if (sameItem(item, "TEXT")) {
			    result[count++] = XA_TEXT(XtDisplay(w));
			} else if (sameItem(item, "COMPOUND_TEXT")) {
			    result[count++] = XA_COMPOUND_TEXT(XtDisplay(w));
			} else if (sameItem(item, "STRING")) {
			    result[count++] = XA_STRING;
			}
			*nextp++ = nextc;
			listp = nextp;
			free(item);
		    } while (nextc != '\0');
		    if (count) {
			result[count] = None;
			override = True;
			*resultp = result;
		    } else {
			XtFree((char *) result);
		    }
		}
		free(copied);
	    } else {
		TRACE(("Couldn't allocate copy of selection types\n"));
	    }
	}
    }
    return override;
}