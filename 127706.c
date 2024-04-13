static inline zend_long parse_iv2(const unsigned char *p, const unsigned char **q)
{
	char cursor;
	zend_long result = 0;
	int neg = 0;

	switch (*p) {
		case '-':
			neg++;
			/* fall-through */
		case '+':
			p++;
	}

	while (1) {
		cursor = (char)*p;
		if (cursor >= '0' && cursor <= '9') {
			result = result * 10 + (size_t)(cursor - (unsigned char)'0');
		} else {
			break;
		}
		p++;
	}
	if (q) *q = p;
	if (neg) return -result;
	return result;
}