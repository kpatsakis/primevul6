void *realloc_strcat(char *ptr, char *s)
{
	size_t old = strlen(ptr), len = strlen(s);
	char *ret;

	if (!len)
		return ptr;

	len += old + 1;
	align_len(&len);

	ret = malloc(len);
	if (unlikely(!ret))
		quit(1, "Failed to malloc in realloc_strcat");

	sprintf(ret, "%s%s", ptr, s);
	free(ptr);
	return ret;
}