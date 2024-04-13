ca_x509_name_unescape(char *src, char *dst, char marker)
{
	while (*src) {
		if (*src == marker) {
			src++;
			break;
		}
		if (*src == '\\') {
			src++;
			if (!*src) {
				log_warnx("%s: '\\' at end of string",
				    __func__);
				*dst = '\0';
				return (NULL);
			}
		}
		*dst++ = *src++;
	}
	*dst = '\0';
	return (src);
}