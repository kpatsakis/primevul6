header_afiol(struct archive_read *a, struct cpio *cpio,
    struct archive_entry *entry, size_t *namelength, size_t *name_pad)
{
	const void *h;
	const char *header;

	a->archive.archive_format = ARCHIVE_FORMAT_CPIO_AFIO_LARGE;
	a->archive.archive_format_name = "afio large ASCII";

	/* Read fixed-size portion of header. */
	h = __archive_read_ahead(a, afiol_header_size, NULL);
	if (h == NULL)
	    return (ARCHIVE_FATAL);

	/* Parse out octal fields. */
	header = (const char *)h;

	archive_entry_set_dev(entry, 
		(dev_t)atol16(header + afiol_dev_offset, afiol_dev_size));
	archive_entry_set_ino(entry, atol16(header + afiol_ino_offset, afiol_ino_size));
	archive_entry_set_mode(entry,
		(mode_t)atol8(header + afiol_mode_offset, afiol_mode_size));
	archive_entry_set_uid(entry, atol16(header + afiol_uid_offset, afiol_uid_size));
	archive_entry_set_gid(entry, atol16(header + afiol_gid_offset, afiol_gid_size));
	archive_entry_set_nlink(entry,
		(unsigned int)atol16(header + afiol_nlink_offset, afiol_nlink_size));
	archive_entry_set_rdev(entry,
		(dev_t)atol16(header + afiol_rdev_offset, afiol_rdev_size));
	archive_entry_set_mtime(entry, atol16(header + afiol_mtime_offset, afiol_mtime_size), 0);
	*namelength = (size_t)atol16(header + afiol_namesize_offset, afiol_namesize_size);
	*name_pad = 0; /* No padding of filename. */

	cpio->entry_bytes_remaining =
	    atol16(header + afiol_filesize_offset, afiol_filesize_size);
	archive_entry_set_size(entry, cpio->entry_bytes_remaining);
	cpio->entry_padding = 0;
	__archive_read_consume(a, afiol_header_size);
	return (ARCHIVE_OK);
}