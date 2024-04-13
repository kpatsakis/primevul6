arrayname(const char *str)
{
	const char *p;
	char *rv;

	if ((p = cstrchr(str, '[')) == 0)
		/* Shouldn't happen, but why worry? */
		strdupx(rv, str, ATEMP);
	else
		strndupx(rv, str, p - str, ATEMP);

	return (rv);
}