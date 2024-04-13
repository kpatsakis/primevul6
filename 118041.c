static int strcmpnull(const char *a, const char *b) {
	if (!a || !b) {
		return -1;
	}
	return strcmp (a, b);
}