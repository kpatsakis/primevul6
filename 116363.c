is_octal(const char *p, size_t len)
{
	while (len-- > 0) {
		if (*p < '0' || *p > '7')
			return (0);
	        ++p;
	}
	return (1);
}