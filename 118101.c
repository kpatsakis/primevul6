feature_alert_opts(op, optn)
char *op;
const char *optn;
{
	char buf[BUFSZ];
	boolean rejectver = FALSE;
	unsigned long fnv = get_feature_notice_ver(op);		/* version.c */
	if (fnv == 0L) return 0;
	if (fnv > get_current_feature_ver())
		rejectver = TRUE;
	else
		flags.suppress_alert = fnv;
	if (rejectver) {
		if (!initial)
			You_cant("disable new feature alerts for future versions.");
		else {
			Sprintf(buf,
				"\n%s=%s Invalid reference to a future version ignored",
				optn, op);
			badoption(buf);
		}
		return 0;
	}
	if (!initial) {
		Sprintf(buf, "%lu.%lu.%lu", FEATURE_NOTICE_VER_MAJ,
			FEATURE_NOTICE_VER_MIN, FEATURE_NOTICE_VER_PATCH);
		pline("Feature change alerts disabled for NetHack %s features and prior.",
			buf);
	}
	return 1;
}