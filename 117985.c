static int getreg(const char *str) {
	int i;
	char *ep;
	const char *aliases[] = { "sl", "fp", "ip", "sp", "lr", "pc", NULL };
	if (!str || !*str) {
		return -1;
	}
	if (*str == 'r') {
		int reg = strtol (str + 1, &ep, 10);
		if ((ep[0] != '\0') || (str[1] == '\0')) {
			return -1;
		}
		if (reg < 16 && reg >= 0) {
			return reg;
		}
	}
	for (i=0; aliases[i]; i++) {
		if (!strcmpnull (str, aliases[i])) {
			return 10 + i;
		}
	}
	return -1;
}