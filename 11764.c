skip_delay(char *s)
{
    while (*s == '/' || isdigit(UChar(*s)))
	++s;
    return s;
}