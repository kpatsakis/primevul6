put_translate(int c)
/* emit a comment char, translating terminfo names to termcap names */
{
    static bool in_name = FALSE;
    static size_t have, used;
    static char *namebuf, *suffix;

    if (in_name) {
	if (used + 1 >= have) {
	    have += 132;
	    if ((namebuf = typeRealloc(char, have, namebuf)) == 0)
		  failed("put_translate namebuf");
	    if ((suffix = typeRealloc(char, have, suffix)) == 0)
		  failed("put_translate suffix");
	}
	if (c == '\n' || c == '@') {
	    namebuf[used++] = '\0';
	    (void) putchar('<');
	    (void) fputs(namebuf, stdout);
	    putchar(c);
	    in_name = FALSE;
	} else if (c != '>') {
	    namebuf[used++] = (char) c;
	} else {		/* ah! candidate name! */
	    char *up;
	    NCURSES_CONST char *tp;

	    namebuf[used++] = '\0';
	    in_name = FALSE;

	    suffix[0] = '\0';
	    if ((up = strchr(namebuf, '#')) != 0
		|| (up = strchr(namebuf, '=')) != 0
		|| ((up = strchr(namebuf, '@')) != 0 && up[1] == '>')) {
		_nc_STRCPY(suffix, up, have);
		*up = '\0';
	    }

	    if ((tp = nametrans(namebuf)) != 0) {
		(void) putchar(':');
		(void) fputs(tp, stdout);
		(void) fputs(suffix, stdout);
		(void) putchar(':');
	    } else {
		/* couldn't find a translation, just dump the name */
		(void) putchar('<');
		(void) fputs(namebuf, stdout);
		(void) fputs(suffix, stdout);
		(void) putchar('>');
	    }
	}
    } else {
	used = 0;
	if (c == '<') {
	    in_name = TRUE;
	} else {
	    putchar(c);
	}
    }
}