char *bin2hex(const unsigned char *p, size_t len)
{
	unsigned int i;
	ssize_t slen;
	char *s;

	slen = len * 2 + 1;
	if (slen % 4)
		slen += 4 - (slen % 4);
	s = calloc(slen, 1);
	if (unlikely(!s))
		quit(1, "Failed to calloc in bin2hex");

	for (i = 0; i < len; i++)
		sprintf(s + (i * 2), "%02x", (unsigned int) p[i]);

	return s;
}