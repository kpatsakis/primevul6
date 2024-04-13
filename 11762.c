show_tty_change(TTY * old_settings,
		TTY * new_settings,
		const char *name,
		int which,
		unsigned def)
{
    unsigned older, newer;
    char *p;

    newer = new_settings->c_cc[which];
    older = old_settings->c_cc[which];

    if (older == newer && older == def)
	return;

    (void) fprintf(stderr, "%s %s ", name, older == newer ? "is" : "set to");

    if (DISABLED(newer)) {
	(void) fprintf(stderr, "undef.\n");
	/*
	 * Check 'delete' before 'backspace', since the key_backspace value
	 * is ambiguous.
	 */
    } else if (newer == 0177) {
	(void) fprintf(stderr, "delete.\n");
    } else if ((p = key_backspace) != 0
	       && newer == (unsigned char) p[0]
	       && p[1] == '\0') {
	(void) fprintf(stderr, "backspace.\n");
    } else if (newer < 040) {
	newer ^= 0100;
	(void) fprintf(stderr, "control-%c (^%c).\n", UChar(newer), UChar(newer));
    } else
	(void) fprintf(stderr, "%c.\n", UChar(newer));
}