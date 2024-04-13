is_wdvarname(const char *s, bool aok)
{
	const char *p = skip_wdvarname(s, aok);

	return (p != s && p[0] == EOS);
}