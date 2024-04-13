_SelectionTargets(Widget w)
{
    Atom *result;
    XtermWidget xw;

    if ((xw = getXtermWidget(w)) == 0) {
	result = NULL;
    } else {
	TScreen *screen = TScreenOf(xw);

#if OPT_WIDE_CHARS
	if (screen->wide_chars) {
	    result = allocUtf8Targets(w, screen);
	} else
#endif
	{
	    /* not screen->wide_chars */
	    result = alloc8bitTargets(w, screen);
	}
    }

    return result;
}