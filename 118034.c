static st32 getreglist(const char *input) {
	st32 res;
	
	if (!input || (strlen (input) < 2) || (*input != '{') || !r_str_endswith (input, "}")) {
		return -1;
	}
	if (*input) {
		input++;
	}
	char *temp = r_str_ndup (input, strlen (input) - 1);
	if (!temp) {
		return -1;
	}
	res = getlistmask (temp);
	free (temp);
	return res;
}