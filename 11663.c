skip_zero(char *s)
{
    if (s[0] == '0') {
	if (s[1] == ';')
	    s += 2;
	else if (isalpha(UChar(s[1])))
	    s += 1;
    }
    return s;
}