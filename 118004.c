static ut64 getnum(const char *str) {
	char *endptr;
	err = false;
	ut64 val;

	if (!str) {
		err = true;
		return 0;
	}
	while (*str == '$' || *str == '#') {
		str++;
	}
	val = strtol (str, &endptr, 0);
	if (str != endptr && *endptr == '\0') {
		return val;
	}
	err = true;
	return 0;
}