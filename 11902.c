allow_gpm_mouse(SCREEN *sp GCC_UNUSED)
{
    bool result = FALSE;

#if USE_WEAK_SYMBOLS
    /* Danger Robinson: do not use dlopen for libgpm if already loaded */
    if ((Gpm_Wgetch) != 0) {
	if (!sp->_mouse_gpm_loaded) {
	    T(("GPM library was already dlopen'd, not by us"));
	}
    } else
#endif
	/* GPM does printf's without checking if stdout is a terminal */
    if (NC_ISATTY(fileno(stdout))) {
	const char *list = getenv("NCURSES_GPM_TERMS");
	const char *env = getenv("TERM");
	if (list != 0) {
	    if (env != 0) {
		result = _nc_name_match(list, env, "|:");
	    }
	} else {
	    /* GPM checks the beginning of the $TERM variable to decide if it
	     * should pass xterm events through.  There is no real advantage in
	     * allowing GPM to do this.  Recent versions relax that check, and
	     * pretend that GPM can work with any terminal having the kmous
	     * capability.  Perhaps that works for someone.  If so, they can
	     * set the environment variable (above).
	     */
	    if (env != 0 && strstr(env, "linux") != 0) {
		result = TRUE;
	    }
	}
    }
    return result;
}