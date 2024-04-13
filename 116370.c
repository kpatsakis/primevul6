archive_read_format_cpio_options(struct archive_read *a,
    const char *key, const char *val)
{
	struct cpio *cpio;
	int ret = ARCHIVE_FAILED;

	cpio = (struct cpio *)(a->format->data);
	if (strcmp(key, "compat-2x")  == 0) {
		/* Handle filnames as libarchive 2.x */
		cpio->init_default_conversion = (val != NULL)?1:0;
		return (ARCHIVE_OK);
	} else if (strcmp(key, "hdrcharset")  == 0) {
		if (val == NULL || val[0] == 0)
			archive_set_error(&a->archive, ARCHIVE_ERRNO_MISC,
			    "cpio: hdrcharset option needs a character-set name");
		else {
			cpio->opt_sconv =
			    archive_string_conversion_from_charset(
				&a->archive, val, 0);
			if (cpio->opt_sconv != NULL)
				ret = ARCHIVE_OK;
			else
				ret = ARCHIVE_FATAL;
		}
		return (ret);
	}

	/* Note: The "warn" return is just to inform the options
	 * supervisor that we didn't handle it.  It will generate
	 * a suitable error if no one used this option. */
	return (ARCHIVE_WARN);
}