getspec(struct tbl *vp)
{
	mksh_ari_u num;
	int st;
	struct timeval tv;

	switch ((st = special(vp->name))) {
	case V_COLUMNS:
	case V_LINES:
		/*
		 * Do NOT export COLUMNS/LINES. Many applications
		 * check COLUMNS/LINES before checking ws.ws_col/row,
		 * so if the app is started with C/L in the environ
		 * and the window is then resized, the app won't
		 * see the change cause the environ doesn't change.
		 */
		if (got_winch)
			change_winsz();
		break;
	}
	switch (st) {
	case V_BASHPID:
		num.u = (mksh_uari_t)procpid;
		break;
	case V_COLUMNS:
		num.i = x_cols;
		break;
	case V_HISTSIZE:
		num.i = histsize;
		break;
	case V_LINENO:
		num.i = current_lineno + user_lineno;
		break;
	case V_LINES:
		num.i = x_lins;
		break;
	case V_EPOCHREALTIME: {
		/* 10(%u) + 1(.) + 6 + NUL */
		char buf[18];

		vp->flag &= ~SPECIAL;
		mksh_TIME(tv);
		shf_snprintf(buf, sizeof(buf), "%u.%06u",
		    (unsigned)tv.tv_sec, (unsigned)tv.tv_usec);
		setstr(vp, buf, KSH_RETURN_ERROR | 0x4);
		vp->flag |= SPECIAL;
		return;
	}
	case V_OPTIND:
		num.i = user_opt.uoptind;
		break;
	case V_RANDOM:
		num.i = rndget();
		break;
	case V_SECONDS:
		/*
		 * On start up the value of SECONDS is used before
		 * it has been set - don't do anything in this case
		 * (see initcoms[] in main.c).
		 */
		if (vp->flag & ISSET) {
			mksh_TIME(tv);
			num.i = tv.tv_sec - seconds;
		} else
			return;
		break;
	default:
		/* do nothing, do not touch vp at all */
		return;
	}
	vp->flag &= ~SPECIAL;
	setint_n(vp, num.i, 0);
	vp->flag |= SPECIAL;
}