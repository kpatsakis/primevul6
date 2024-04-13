static void smk_bu_mode(int mode, char *s)
{
	int i = 0;

	if (mode & MAY_READ)
		s[i++] = 'r';
	if (mode & MAY_WRITE)
		s[i++] = 'w';
	if (mode & MAY_EXEC)
		s[i++] = 'x';
	if (mode & MAY_APPEND)
		s[i++] = 'a';
	if (mode & MAY_TRANSMUTE)
		s[i++] = 't';
	if (mode & MAY_LOCK)
		s[i++] = 'l';
	if (i == 0)
		s[i++] = '-';
	s[i] = '\0';
}