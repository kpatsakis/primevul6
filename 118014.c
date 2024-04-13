static int getcoprocreg(const char *str) {
	char *ep;
	
	if (!str || !*str) {
		return -1;
	}
	if (r_str_startswith (str, "c")) {
		int reg = strtol (str + 1, &ep, 10);
		if ((ep[0] != '\0') || (str[1] == '\0')) {
			return -1;
		}
		if (reg < 16 && reg >= 0) {
			return reg;
		}
	}
	return -1;
}