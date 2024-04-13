static st32 getnummemendbang (const char *input) {
	st32 res;
	err = false;
	if (!input || (strlen (input) < 2) || (input[strlen(input) - 2] != ']' || !r_str_endswith (input, "!"))) {
		err = true;
		return 0;
	}
	char *temp = r_str_ndup (input, strlen (input) - 2);
	if (!temp) {
		err = true;
		return 0;
	}
	res = getnum (temp);
	free (temp);
	return res;
}