static size_t php_strnlen(char* str, size_t maxlen) {
	size_t len = 0;

	if (str && maxlen && *str) {
		do {
			len++;
		} while (--maxlen && *(++str));
	}
	return len;
}