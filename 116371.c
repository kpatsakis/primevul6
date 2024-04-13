archive_read_format_cpio_read_data(struct archive_read *a,
    const void **buff, size_t *size, int64_t *offset)
{
	ssize_t bytes_read;
	struct cpio *cpio;

	cpio = (struct cpio *)(a->format->data);

	if (cpio->entry_bytes_unconsumed) {
		__archive_read_consume(a, cpio->entry_bytes_unconsumed);
		cpio->entry_bytes_unconsumed = 0;
	}

	if (cpio->entry_bytes_remaining > 0) {
		*buff = __archive_read_ahead(a, 1, &bytes_read);
		if (bytes_read <= 0)
			return (ARCHIVE_FATAL);
		if (bytes_read > cpio->entry_bytes_remaining)
			bytes_read = (ssize_t)cpio->entry_bytes_remaining;
		*size = bytes_read;
		cpio->entry_bytes_unconsumed = bytes_read;
		*offset = cpio->entry_offset;
		cpio->entry_offset += bytes_read;
		cpio->entry_bytes_remaining -= bytes_read;
		return (ARCHIVE_OK);
	} else {
		if (cpio->entry_padding !=
			__archive_read_consume(a, cpio->entry_padding)) {
			return (ARCHIVE_FATAL);
		}
		cpio->entry_padding = 0;
		*buff = NULL;
		*size = 0;
		*offset = cpio->entry_offset;
		return (ARCHIVE_EOF);
	}
}