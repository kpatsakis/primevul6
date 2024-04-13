skip_delay(const char *s)
{
    if (s[0] == '$' && s[1] == '<') {
	s += 2;
	while (isdigit(UChar(*s)) || *s == '/')
	    ++s;
	if (*s == '>')
	    ++s;
    }
    return s;
}