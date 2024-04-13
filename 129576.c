archive_read_support_format_zip(struct archive *a)
{
	int r;
	r = archive_read_support_format_zip_streamable(a);
	if (r != ARCHIVE_OK)
		return r;
	return (archive_read_support_format_zip_seekable(a));
}