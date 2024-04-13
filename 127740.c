setspec(struct tbl *vp)
{
	mksh_ari_u num;
	char *s;
	int st;

	switch ((st = special(vp->name))) {
#if HAVE_PERSISTENT_HISTORY
	case V_HISTFILE:
		sethistfile(str_val(vp));
		return;
#endif
	case V_IFS:
		setctypes(s = str_val(vp), C_IFS);
		ifs0 = *s;
		return;
	case V_PATH:
		if (path)
			afree(path, APERM);
		s = str_val(vp);
		strdupx(path, s, APERM);
		/* clear tracked aliases */
		flushcom(true);
		return;
	case V_TMPDIR:
		if (tmpdir) {
			afree(tmpdir, APERM);
			tmpdir = NULL;
		}
		/*
		 * Use tmpdir iff it is an absolute path, is writable
		 * and searchable and is a directory...
		 */
		{
			struct stat statb;

			s = str_val(vp);
			/* LINTED use of access */
			if (s[0] == '/' && access(s, W_OK|X_OK) == 0 &&
			    stat(s, &statb) == 0 && S_ISDIR(statb.st_mode))
				strdupx(tmpdir, s, APERM);
		}
		return;
	/* common sub-cases */
	case V_COLUMNS:
	case V_LINES:
		if (vp->flag & IMPORT) {
			/* do not touch */
			unspecial(vp->name);
			vp->flag &= ~SPECIAL;
			return;
		}
		/* FALLTHROUGH */
	case V_HISTSIZE:
	case V_LINENO:
	case V_OPTIND:
	case V_RANDOM:
	case V_SECONDS:
	case V_TMOUT:
		vp->flag &= ~SPECIAL;
		if (getint(vp, &num, false) == -1) {
			s = str_val(vp);
			if (st != V_RANDOM)
				errorf("%s: %s: %s", vp->name, "bad number", s);
			num.u = hash(s);
		}
		vp->flag |= SPECIAL;
		break;
	default:
		/* do nothing, do not touch vp at all */
		return;
	}

	/* process the singular parts of the common cases */

	switch (st) {
	case V_COLUMNS:
		if (num.i >= MIN_COLS)
			x_cols = num.i;
		break;
	case V_HISTSIZE:
		sethistsize(num.i);
		break;
	case V_LINENO:
		/* The -1 is because line numbering starts at 1. */
		user_lineno = num.u - current_lineno - 1;
		break;
	case V_LINES:
		if (num.i >= MIN_LINS)
			x_lins = num.i;
		break;
	case V_OPTIND:
		getopts_reset((int)num.i);
		break;
	case V_RANDOM:
		/*
		 * mksh R39d+ no longer has the traditional repeatability
		 * of $RANDOM sequences, but always retains state
		 */
		rndset((unsigned long)num.u);
		break;
	case V_SECONDS:
		{
			struct timeval tv;

			mksh_TIME(tv);
			seconds = tv.tv_sec - num.i;
		}
		break;
	case V_TMOUT:
		ksh_tmout = num.i >= 0 ? num.i : 0;
		break;
	}
}