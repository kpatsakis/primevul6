nh_getenv(ev)
const char *ev;
{
	char *getev = getenv(ev);

	if (getev && strlen(getev) <= (BUFSZ / 2))
		return getev;
	else
		return (char *)0;
}