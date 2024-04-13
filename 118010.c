static ut64 getnumbang(const char *str) {
	ut64 res;

	if (!str || !*str || !r_str_endswith (str, "!")) {
		err = true;
		return 0;
	}
	char *temp = r_str_ndup (str, strlen (str) - 1);
	if (!temp) {
		return -1;
	}
	err = false;
	res = getnum (temp);
	free (temp);
	return res; // err propagates
}