static st32 getregmemstartend(const char *input) {
	st32 res;
	if (!input || (strlen (input) < 2) || (*input != '[') || !r_str_endswith (input, "]")) {
		return -1;
	}
	input++;
	char *temp = r_str_ndup (input, strlen (input) - 1);
	if (!temp) {
		return -1;
	}
	res = getreg (temp);
	free (temp);
	return res;
}