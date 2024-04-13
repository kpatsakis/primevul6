wc_set_window_colors(op)
char *op;
{
	/* syntax:
	 *  menu white/black message green/yellow status white/blue text white/black
	 */

	int j;
	char buf[BUFSZ];
	char *wn, *tfg, *tbg, *newop;
	static const char *wnames[] = { "menu", "message", "status", "text" };
	static const char *shortnames[] = { "mnu", "msg", "sts", "txt" };
	static char **fgp[] = {
		&iflags.wc_foregrnd_menu,
		&iflags.wc_foregrnd_message,
		&iflags.wc_foregrnd_status,
		&iflags.wc_foregrnd_text
	};
	static char **bgp[] = {
		&iflags.wc_backgrnd_menu,
		&iflags.wc_backgrnd_message,
		&iflags.wc_backgrnd_status,
		&iflags.wc_backgrnd_text
	};

	Strcpy(buf, op);
	newop = mungspaces(buf);
	while (newop && *newop) {

		wn = tfg = tbg = (char *)0;

		/* until first non-space in case there's leading spaces - before colorname*/
		while(*newop && isspace(*newop)) newop++;
		if (*newop) wn = newop;
		else return 0;

		/* until first space - colorname*/
		while(*newop && !isspace(*newop)) newop++;
		if (*newop) *newop = '\0';
		else return 0;
		newop++;

		/* until first non-space - before foreground*/
		while(*newop && isspace(*newop)) newop++;
		if (*newop) tfg = newop;
		else return 0;

		/* until slash - foreground */
		while(*newop && *newop != '/') newop++;
		if (*newop) *newop = '\0';
		else return 0;
		newop++;

		/* until first non-space (in case there's leading space after slash) - before background */
		while(*newop && isspace(*newop)) newop++;
		if (*newop) tbg = newop;
		else return 0;

		/* until first space - background */
		while(*newop && !isspace(*newop)) newop++;
		if (*newop) *newop++ = '\0';

		for (j = 0; j < 4; ++j) {
			if (!strcmpi(wn, wnames[j]) ||
			    !strcmpi(wn, shortnames[j])) {
				if (tfg && !strstri(tfg, " ")) {
					if (*fgp[j]) free(*fgp[j]);
					*fgp[j] = (char *)alloc(strlen(tfg) + 1);
					Strcpy(*fgp[j], tfg);
				}
				if (tbg && !strstri(tbg, " ")) {
					if (*bgp[j]) free(*bgp[j]);
					*bgp[j] = (char *)alloc(strlen(tbg) + 1);
					Strcpy(*bgp[j], tbg);
				}
				break;
			}
		}
	}
	return 1;
}