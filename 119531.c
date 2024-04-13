static int mystrcmp(char *s1, char *s2) {
	if (s1 == NULL && s2 == NULL)
		return 0;
	if (s1 == NULL)
		return -1;
	if (s2 == NULL)
		return 1;
	return strcmp(s1, s2);
}