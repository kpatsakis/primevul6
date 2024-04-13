skip_varname(const char *s, bool aok)
{
	size_t alen;

	if (s && ksh_isalphx(*s)) {
		while (*++s && ksh_isalnux(*s))
			;
		if (aok && *s == '[' && (alen = array_ref_len(s)))
			s += alen;
	}
	return (s);
}