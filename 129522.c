archive_read_support_format_zip_capabilities_streamable(struct archive_read * a)
{
	(void)a; /* UNUSED */
	return (ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_DATA |
		ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_METADATA);
}