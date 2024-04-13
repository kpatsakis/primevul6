match_repeated_char(const unsigned char *data, unsigned char ch, size_t n)
{
	int i;
	for (i=n; i; --i, ++data) {
		if (*data == 0) {
			i = 0;	/* all bytes before NUL matched */
			break;
		}
		if (*data != ch)
			break;
	}
	return i;
}