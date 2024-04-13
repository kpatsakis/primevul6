header_odc(struct archive_read *a, struct cpio *cpio,
    struct archive_entry *entry, size_t *namelength, size_t *name_pad)
{
	const void *h;
	int r;
	const char *header;

	a->archive.archive_format = ARCHIVE_FORMAT_CPIO_POSIX;
	a->archive.archive_format_name = "POSIX octet-oriented cpio";

	/* Find the start of the next header. */
	r = find_odc_header(a);
	if (r < ARCHIVE_WARN)
		return (r);

	if (a->archive.archive_format == ARCHIVE_FORMAT_CPIO_AFIO_LARGE) {
		int r2 = (header_afiol(a, cpio, entry, namelength, name_pad));
		if (r2 == ARCHIVE_OK)
			return (r);
		else
			return (r2);
	}

	/* Read fixed-size portion of header. */
	h = __archive_read_ahead(a, odc_header_size, NULL);
	if (h == NULL)
	    return (ARCHIVE_FATAL);

	/* Parse out octal fields. */
	header = (const char *)h;

	archive_entry_set_dev(entry, 
		(dev_t)atol8(header + odc_dev_offset, odc_dev_size));
	archive_entry_set_ino(entry, atol8(header + odc_ino_offset, odc_ino_size));
	archive_entry_set_mode(entry, 
		(mode_t)atol8(header + odc_mode_offset, odc_mode_size));
	archive_entry_set_uid(entry, atol8(header + odc_uid_offset, odc_uid_size));
	archive_entry_set_gid(entry, atol8(header + odc_gid_offset, odc_gid_size));
	archive_entry_set_nlink(entry, 
		(unsigned int)atol8(header + odc_nlink_offset, odc_nlink_size));
	archive_entry_set_rdev(entry,
		(dev_t)atol8(header + odc_rdev_offset, odc_rdev_size));
	archive_entry_set_mtime(entry, atol8(header + odc_mtime_offset, odc_mtime_size), 0);
	*namelength = (size_t)atol8(header + odc_namesize_offset, odc_namesize_size);
	*name_pad = 0; /* No padding of filename. */

	/*
	 * Note: entry_bytes_remaining is at least 64 bits and
	 * therefore guaranteed to be big enough for a 33-bit file
	 * size.
	 */
	cpio->entry_bytes_remaining =
	    atol8(header + odc_filesize_offset, odc_filesize_size);
	archive_entry_set_size(entry, cpio->entry_bytes_remaining);
	cpio->entry_padding = 0;
	__archive_read_consume(a, odc_header_size);
	return (r);
}