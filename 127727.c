unsetspec(struct tbl *vp)
{
	/*
	 * AT&T ksh man page says OPTIND, OPTARG and _ lose special
	 * meaning, but OPTARG does not (still set by getopts) and _ is
	 * also still set in various places. Don't know what AT&T does
	 * for HISTSIZE, HISTFILE. Unsetting these in AT&T ksh does not
	 * loose the 'specialness': IFS, COLUMNS, PATH, TMPDIR
	 */

	switch (special(vp->name)) {
	case V_IFS:
		setctypes(TC_IFSWS, C_IFS);
		ifs0 = ' ';
		break;
	case V_PATH:
		if (path)
			afree(path, APERM);
		strdupx(path, def_path, APERM);
		/* clear tracked aliases */
		flushcom(true);
		break;
	case V_TMPDIR:
		/* should not become unspecial */
		if (tmpdir) {
			afree(tmpdir, APERM);
			tmpdir = NULL;
		}
		break;
	case V_LINENO:
	case V_RANDOM:
	case V_SECONDS:
	case V_TMOUT:
		/* AT&T ksh leaves previous value in place */
		unspecial(vp->name);
		break;
	}
}