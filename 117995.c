static int getcoproc(const char *str) {
	char *ep;
	if (!str || !*str) {
		return -1;
	}
	if (*str == 'p') {
		int coproc = strtol (str + 1, &ep, 10);
		if ((ep[0] != '\0') || (str[1] == '\0')) {
			return -1;
		}
		if (coproc < 16 && coproc >= 0) {
			return coproc;
		}
	}
	return -1;
}