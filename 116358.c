archive_read_support_format_cpio(struct archive *_a)
{
	struct archive_read *a = (struct archive_read *)_a;
	struct cpio *cpio;
	int r;

	archive_check_magic(_a, ARCHIVE_READ_MAGIC,
	    ARCHIVE_STATE_NEW, "archive_read_support_format_cpio");

	cpio = (struct cpio *)calloc(1, sizeof(*cpio));
	if (cpio == NULL) {
		archive_set_error(&a->archive, ENOMEM, "Can't allocate cpio data");
		return (ARCHIVE_FATAL);
	}
	cpio->magic = CPIO_MAGIC;

	r = __archive_read_register_format(a,
	    cpio,
	    "cpio",
	    archive_read_format_cpio_bid,
	    archive_read_format_cpio_options,
	    archive_read_format_cpio_read_header,
	    archive_read_format_cpio_read_data,
	    archive_read_format_cpio_skip,
	    NULL,
	    archive_read_format_cpio_cleanup,
	    NULL,
	    NULL);

	if (r != ARCHIVE_OK)
		free(cpio);
	return (ARCHIVE_OK);
}