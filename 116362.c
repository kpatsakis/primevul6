is_hex(const char *p, size_t len)
{
	while (len-- > 0) {
		if ((*p >= '0' && *p <= '9')
		    || (*p >= 'a' && *p <= 'f')
		    || (*p >= 'A' && *p <= 'F'))
			++p;
		else
			return (0);
	}
	return (1);
}