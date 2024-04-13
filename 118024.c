static st32 getshiftmemend(const char *input) {
	st32 res;
	if (!input || !*input || !r_str_endswith (input, "]")) {
		return -1;
	}

	char *temp = r_str_ndup (input, strlen (input) - 1);
	if (!temp) {
		return -1;
	}
	res = thumb_getshift (temp);
	free (temp);
	return res;
}