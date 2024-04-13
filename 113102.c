ca_x509_name_parse(char *subject)
{
	char		*cp, *value = NULL, *type = NULL;
	size_t		 maxlen;
	X509_NAME	*name = NULL;

	if (*subject != '/') {
		log_warnx("%s: leading '/' missing in '%s'", __func__, subject);
		goto err;
	}

	/* length of subject is upper bound for unescaped type/value */
	maxlen = strlen(subject) + 1;

	if ((type = calloc(1, maxlen)) == NULL ||
	    (value = calloc(1, maxlen)) == NULL ||
	    (name = X509_NAME_new()) == NULL)
		goto err;

	cp = subject + 1;
	while (*cp) {
		/* unescape type, terminated by '=' */
		cp = ca_x509_name_unescape(cp, type, '=');
		if (cp == NULL) {
			log_warnx("%s: could not parse type", __func__);
			goto err;
		}
		if (!*cp) {
			log_warnx("%s: missing value", __func__);
			goto err;
		}
		/* unescape value, terminated by '/' */
		cp = ca_x509_name_unescape(cp, value, '/');
		if (cp == NULL) {
			log_warnx("%s: could not parse value", __func__);
			goto err;
		}
		if (!*type || !*value) {
			log_warnx("%s: empty type or value", __func__);
			goto err;
		}
		log_debug("%s: setting '%s' to '%s'", __func__, type, value);
		if (!X509_NAME_add_entry_by_txt(name, type, MBSTRING_ASC,
		    value, -1, -1, 0)) {
			log_warnx("%s: setting '%s' to '%s' failed", __func__,
			    type, value);
			ca_sslerror(__func__);
			goto err;
		}
	}
	free(type);
	free(value);
	return (name);

err:
	X509_NAME_free(name);
	free(type);
	free(value);
	return (NULL);
}