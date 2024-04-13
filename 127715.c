array_ref_len(const char *cp)
{
	const char *s = cp;
	char c;
	int depth = 0;

	while ((c = *s++) && (c != ']' || --depth))
		if (c == '[')
			depth++;
	if (!c)
		return (0);
	return (s - cp);
}