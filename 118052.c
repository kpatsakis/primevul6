static st32 getnummemend (const char *input) {
	st32 res;
	err = false;
	if (!input || !*input || !r_str_endswith (input, "]")) {
		err = true;
		return -1;
	}
	char *temp = r_str_ndup (input, strlen (input) - 1);
	if (!temp) {
		err = true;
		return -1;
	}
	res = getnum (temp);
	free (temp);
	return res;
}