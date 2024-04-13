header_bin_be(struct archive_read *a, struct cpio *cpio,
    struct archive_entry *entry, size_t *namelength, size_t *name_pad)
{
	const void *h;
	const unsigned char *header;

	a->archive.archive_format = ARCHIVE_FORMAT_CPIO_BIN_BE;
	a->archive.archive_format_name = "cpio (big-endian binary)";

	/* Read fixed-size portion of header. */
	h = __archive_read_ahead(a, bin_header_size, NULL);
	if (h == NULL) {
	    archive_set_error(&a->archive, 0,
		"End of file trying to read next cpio header");
	    return (ARCHIVE_FATAL);
	}

	/* Parse out binary fields. */
	header = (const unsigned char *)h;

	archive_entry_set_dev(entry, header[bin_dev_offset] * 256 + header[bin_dev_offset + 1]);
	archive_entry_set_ino(entry, header[bin_ino_offset] * 256 + header[bin_ino_offset + 1]);
	archive_entry_set_mode(entry, header[bin_mode_offset] * 256 + header[bin_mode_offset + 1]);
	archive_entry_set_uid(entry, header[bin_uid_offset] * 256 + header[bin_uid_offset + 1]);
	archive_entry_set_gid(entry, header[bin_gid_offset] * 256 + header[bin_gid_offset + 1]);
	archive_entry_set_nlink(entry, header[bin_nlink_offset] * 256 + header[bin_nlink_offset + 1]);
	archive_entry_set_rdev(entry, header[bin_rdev_offset] * 256 + header[bin_rdev_offset + 1]);
	archive_entry_set_mtime(entry, be4(header + bin_mtime_offset), 0);
	*namelength = header[bin_namesize_offset] * 256 + header[bin_namesize_offset + 1];
	*name_pad = *namelength & 1; /* Pad to even. */

	cpio->entry_bytes_remaining = be4(header + bin_filesize_offset);
	archive_entry_set_size(entry, cpio->entry_bytes_remaining);
	cpio->entry_padding = cpio->entry_bytes_remaining & 1; /* Pad to even. */
	    __archive_read_consume(a, bin_header_size);
	return (ARCHIVE_OK);
}