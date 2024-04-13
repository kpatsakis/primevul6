UnmapSelections(XtermWidget xw)
{
    TScreen *screen = TScreenOf(xw);
    Cardinal n;

    if (screen->mappedSelect) {
	for (n = 0; screen->mappedSelect[n] != 0; ++n)
	    free((void *) screen->mappedSelect[n]);
	FreeAndNull(screen->mappedSelect);
    }
}