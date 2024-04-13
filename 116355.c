archive_read_format_cpio_skip(struct archive_read *a)
{
	struct cpio *cpio = (struct cpio *)(a->format->data);
	int64_t to_skip = cpio->entry_bytes_remaining + cpio->entry_padding +
		cpio->entry_bytes_unconsumed;

	if (to_skip != __archive_read_consume(a, to_skip)) {
		return (ARCHIVE_FATAL);
	}
	cpio->entry_bytes_remaining = 0;
	cpio->entry_padding = 0;
	cpio->entry_bytes_unconsumed = 0;
	return (ARCHIVE_OK);
}