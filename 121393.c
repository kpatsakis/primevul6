removeControls(XtermWidget xw, char *value)
{
    TScreen *screen = TScreenOf(xw);
    size_t dst = 0;

    if (screen->allowPasteControls) {
	dst = strlen(value);
    } else {
	size_t src = 0;
	while ((value[dst] = value[src]) != '\0') {
	    int ch = CharOf(value[src++]);

#define ReplacePaste(n) \
	    if (screen->disallow_paste_controls[n]) \
		value[dst] = ' '

	    if (ch < 32) {
		ReplacePaste(epC0);
		ReplacePaste(ch);
		++dst;
	    } else if (ch == ANSI_DEL) {
		ReplacePaste(epDEL);
		++dst;
	    }
#if OPT_WIDE_CHARS
	    else if (screen->utf8_inparse || screen->utf8_nrc_mode)
		++dst;
#endif
#if OPT_C1_PRINT || OPT_WIDE_CHARS
	    else if (screen->c1_printable)
		++dst;
#endif
	    else if (ch >= 128 && ch < 160)
		continue;
	    else
		++dst;
	}
    }
    return dst;
}